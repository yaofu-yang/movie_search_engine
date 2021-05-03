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

#include "Movie.h"


Movie* CreateMovie() {
  Movie *mov = (Movie*)malloc(sizeof(Movie));
  if (mov == NULL) {
    printf("Couldn't allocate more memory to create a Movie\n");
    return NULL;
  }

  mov->title = NULL;
  mov->star_rating = 0;
  mov->content_rating = NULL;
  mov->genre = NULL;
  mov->duration = 0;
  mov->actor_list = NULL;
  mov->num_actors = 0;

  return mov;
}

void DestroyMovie(Movie* movie) {
  free(movie->title);
  movie->title = NULL;
  movie->star_rating = 0;
  free(movie->content_rating);
  movie->content_rating = NULL;
  free(movie->genre);
  movie->genre = NULL;
  movie->duration = 0;

  while (movie->num_actors > 0) {
    free(movie->actor_list[movie->num_actors - 1]);
    movie->actor_list[--movie->num_actors] = NULL;
  }

  free(movie->actor_list);
  movie->actor_list = NULL;
  free(movie);
  movie = NULL;
}


