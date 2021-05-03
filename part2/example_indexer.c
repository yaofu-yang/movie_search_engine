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
#include <assert.h>
#include <string.h>

#include <ctype.h>
#include <unistd.h>


#include "htll/LinkedList.h"
#include "MovieIndex.h"
#include "Movie.h"
#include "MovieReport.h"
#include "./FileParser.h"

#define kNumArguments 3

void DestroyNothing(void* thing) {
  // Helper method to destroy the LinkedList.
}

int main(int argc, char* argv[]) {
  if (argc != kNumArguments) {
    return 1;
  }
  enum IndexField field;
  char* filename = argv[2];

  int flag = argv[1][1];
  switch (flag) {
    case 'a':
      field = Actor;
      break;
    case 'g':
      field = Genre;
      break;
    case 'c':
      field = ContentRating;
      break;
    case 's':
      field = StarRating;
      break;

    default:
      printf("Please enter one of the appropriate flags: -a -g -c -s");
      return 1;
    }

  LinkedList movie_list  = ReadFile(filename);
  Index index = BuildMovieIndex(movie_list, field);
  PrintReport(index);
  DestroyLinkedList(movie_list, &DestroyNothing);
  DestroyIndex(index);

  return 0;
}
