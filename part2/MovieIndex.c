/*
 *  Adrienne Slaughter
 *  5007 Spr 2020
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  See <http://www.gnu.org/licenses/>.
 */

/////////////////
// Yao-Fu Yang //
//  3/22/2020  //
/////////////////

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#include "MovieIndex.h"
#include "htll/LinkedList.h"
#include "htll/Hashtable.h"
#include "Movie.h"
#include "MovieSet.h"

void DestroyMovieSetWrapper(void *movie_set) {
  DestroyMovieSet((MovieSet)movie_set);
}

void toLower(char *str, int len) {
  for (int i = 0; i < len; i++) {
    str[i] = tolower(str[i]);
  }
}

Index BuildMovieIndex(LinkedList movies, enum IndexField field_to_index) {
  Index movie_index = CreateIndex();
  int num_of_movies = NumElementsInLinkedList(movies);
  if (num_of_movies == 0) {
    return movie_index;
  }

  LLIter iter = CreateLLIter(movies);
  Movie* cur_movie;
  LLIterGetPayload(iter, (void**)&cur_movie);

  AddMovieToIndex(movie_index, cur_movie, field_to_index);
  DestroyMovie(cur_movie);

  while (LLIterHasNext(iter)) {
    LLIterNext(iter);
    LLIterGetPayload(iter, (void**)&cur_movie);
    AddMovieToIndex(movie_index, cur_movie, field_to_index);
    DestroyMovie(cur_movie);
  }
  DestroyLLIter(iter);
  return movie_index;
}


int AddMovieActorsToIndex(Index index, Movie *movie) {
  HTKeyValue kvp;
  HTKeyValue old_kvp;

  for (int i = 0; i < movie->num_actors; i++) {
    kvp.key = ComputeKey(movie, Actor, i);
    if (LookupInHashtable(index, kvp.key, &old_kvp) != 0) {
      MovieSet newSet = CreateMovieSet(movie->actor_list[i]);
      kvp.value = newSet;
      PutInHashtable(index, kvp, &old_kvp);
    } else {
      MovieSet set = (MovieSet)old_kvp.value;
      LLIter iter = CreateLLIter(set->movies);
      void* payload;
      int flag = 0;
      while (flag == 0) {
        LLIterGetPayload(iter, &payload);
        char* cur_title = ((Movie*)payload)->title;
        char* mov_title = movie->title;
        if (strcmp(cur_title, mov_title) == 0) {
          DestroyLLIter(iter);
          return 1;
        }
        if (LLIterHasNext(iter) == 1) {
          LLIterNext(iter);
        } else { flag = 1; }
      }
      kvp.key = old_kvp.key;
      kvp.value = old_kvp.value;
      DestroyLLIter(iter);
    }
    AddMovieToSet((MovieSet)kvp.value, movie);
  }
  return 0;
}

int AddMovieToIndex(Index index, Movie *movie, enum IndexField field) {
  char rating_str[10];
  char *desc;
  if (field == Actor) {
    return AddMovieActorsToIndex(index, movie);
  } else if (field == Genre) {
    desc = movie->genre;
  } else if (field == StarRating) {
    snprintf(rating_str, sizeof(rating_str), "%f", movie->star_rating);
    desc = rating_str;
  } else if (field == ContentRating) {
    desc = movie->content_rating;
  }

  HTKeyValue kvp;
  HTKeyValue old_kvp;
  kvp.key = ComputeKey(movie, field, 0);
  if (LookupInHashtable(index, kvp.key, &old_kvp) != 0) {
    // Index does not contain key.
    MovieSet newSet = CreateMovieSet(desc);
    kvp.value = newSet;
    PutInHashtable(index, kvp, &old_kvp);
  } else {
    MovieSet set = (MovieSet)old_kvp.value;
    LLIter iter = CreateLLIter(set->movies);
    void* payload;
    int flag = 0;
    while (flag == 0) {
      LLIterGetPayload(iter, &payload);
      char* cur_title = ((Movie*)payload)->title;
      char* mov_title = movie->title;
      if (strcmp(cur_title, mov_title) == 0) {
        // Index contains key and the movie in its MovieSet.
        DestroyLLIter(iter);
        return 1;
      }
      if (LLIterHasNext(iter) == 1) {
        LLIterNext(iter);
      } else { flag = 1; }
    }
    // Index contains key but not the movie in its MovieSet.
    kvp.key = old_kvp.key;
    kvp.value = old_kvp.value;
    DestroyLLIter(iter);
  }

  AddMovieToSet((MovieSet)kvp.value, movie);
  return 0;
}

uint64_t ComputeKey(Movie* movie, enum IndexField which_field,
                                  int which_actor) {
  char rating_str[10];
  switch (which_field) {
    case Genre:
      return FNVHash64((unsigned char*)movie->genre, strlen(movie->genre));
    case StarRating:
      snprintf(rating_str, sizeof(rating_str), "%f", movie->star_rating);
      return FNVHash64((unsigned char*)rating_str, strlen(rating_str));
    case ContentRating:
      return FNVHash64((unsigned char*)movie->content_rating,
                       strlen(movie->content_rating));
    case Actor:
      if (which_actor < movie->num_actors) {
        return FNVHash64((unsigned char*)movie->actor_list[which_actor],
                         strlen(movie->actor_list[which_actor]));
      }
      break;
  }
  return -1u;
}


int DestroyIndex(Index index) {
  DestroyHashtable(index, &DestroyMovieSetWrapper);
  return 0;
}

Index CreateIndex() {
  return CreateHashtable(128);
}
