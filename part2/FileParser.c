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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "FileParser.h"
#include "Movie.h"
#include "MovieIndex.h"

#define NUM_FIELDS 6
#define MAX_ROW_LENGTH 1000
#define MAX_NUM_ACTORS 10

char* CheckAndAllocateString(char* token) {
  if (token == NULL) {
    return NULL;
  } else {
    char *out = (char *) malloc((strlen(token) + 1) * sizeof(out));
    snprintf(out, strlen(token) + 1, "%s", token);
    return out;
  }
}

int CheckInt(char* token) {
  if (strcmp("-", token) == 0) {
    return -1;
  } else {
    return atoi(token);
  }
}

double CheckDouble(char* token) {
  if (strcmp("-", token) == 0) {
    return -1;
  } else {
    return atof(token);
  }
}

Movie* CreateMovieFromRow(char *data_row) {
  Movie* mov = CreateMovie();
  if (mov == NULL) {
    printf("Couldn't create a Movie.\n");
    return NULL;
  }
  char* group_delimiter = "|";
  char* actor_delimiter = ",";

  mov->star_rating = CheckDouble(strtok(data_row, group_delimiter));
  mov->title = CheckAndAllocateString(strtok(NULL, group_delimiter));
  mov->content_rating = CheckAndAllocateString(strtok(NULL, group_delimiter));
  mov->genre = CheckAndAllocateString(strtok(NULL, group_delimiter));
  mov->duration = CheckInt(strtok(NULL, group_delimiter));

  char* actors = CheckAndAllocateString(strtok(NULL, group_delimiter));
  mov->actor_list = (char**)malloc(sizeof(char*) * MAX_NUM_ACTORS);

  int index = 0;
  char* token = CheckAndAllocateString(strtok(actors, actor_delimiter));
  while (token != NULL) {
    mov->actor_list[index++] = token;
    mov->num_actors++;
    token = CheckAndAllocateString(strtok(NULL, actor_delimiter));
  }
  free(actors);

  return mov;
}

// Returns a LinkedList of Movie structs from the specified file
LinkedList ReadFile(char* filename) {
  FILE *cfPtr;

  LinkedList movie_list = CreateLinkedList();

  if ((cfPtr = fopen(filename, "r")) == NULL) {
    printf("File could not be opened\n");
    DestroyLinkedList(movie_list, NULL);
    return NULL;
  } else {
    char* row = NULL;
    ssize_t read;
    size_t len = 0;

    while ((read = getline(&row, &len, cfPtr)) != -1) {
      // Got the line; create a movie from it
      MoviePtr movie = CreateMovieFromRow(row);
      if (movie != NULL) {
        InsertLinkedList(movie_list, movie);
      }
    }
    free(row);
    fclose(cfPtr);
  }
  return movie_list;
}
