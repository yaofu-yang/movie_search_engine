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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MovieSet.h"
#include "Movie.h"
#include "htll/LinkedList.h"

#define kNumActors 40

void DestroyMovieWrapper(void *a_movie) {
  DestroyMovie((Movie*)a_movie);
}

// Brought over from FileParser.h
char* AllocateString(char* token) {
  if (token == NULL) {
    return NULL;
  } else {
    char *out = (char *) malloc((strlen(token) + 1) * sizeof(out));
    snprintf(out, strlen(token) + 1, "%s", token);
    return out;
  }
}

int AddMovieToSet(MovieSet set, Movie *movie) {
  Movie* mov = CreateMovie();
  mov->title = AllocateString(movie->title);
  mov->star_rating = movie->star_rating;
  mov->content_rating = AllocateString(movie->content_rating);
  mov->genre = AllocateString(movie->genre);
  mov->duration = movie->duration;
  mov->actor_list = (char**)malloc(sizeof(char*) * kNumActors);
  for (int i = 0; i < movie->num_actors; i++) {
    mov->actor_list[i] = AllocateString(movie->actor_list[i]);
  }
  mov->num_actors = movie->num_actors;
  return InsertLinkedList(set->movies, mov);
}


MovieSet CreateMovieSet(char *desc) {
  MovieSet set = (MovieSet)malloc(sizeof(struct movieSet));
  if (set == NULL) {
    // Out of memory
    printf("Couldn't malloc for movieSet %s\n", desc);
    return NULL;
  }

  set->desc = (char*)malloc((strlen(desc) + 1) * sizeof(set->desc));

  if (set->desc == NULL) {
    printf("Couldn't malloc for movieSet->desc");
    return NULL;
  }
  snprintf(set->desc, strlen(desc) + 1, "%s", desc);
  set->movies = CreateLinkedList();
  return set;
}

void DestroyMovieSet(MovieSet set) {
  void (*fpDestroyMovie)(Movie* movie);
  fpDestroyMovie = &DestroyMovie;
  DestroyLinkedList(set->movies, (void*)fpDestroyMovie);
  free(set->desc);
  free(set);
}
