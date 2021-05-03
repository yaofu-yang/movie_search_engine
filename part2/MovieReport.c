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

#include "MovieIndex.h"
#include "MovieReport.h"
#include "Movie.h"
#include "MovieSet.h"
#include "htll/LinkedList.h"
#include "htll/Hashtable.h"


void PrintReport(Index index) {
  // Create Iter
  HTIter iter = CreateHashtableIterator(index);

  HTKeyValue movie_set;

  HTIteratorGet(iter, &movie_set);
  OutputMovieSet((MovieSet)movie_set.value);

  while (HTIteratorHasMore(iter)) {
    HTIteratorNext(iter);
    HTIteratorGet(iter, &movie_set);
    OutputMovieSet((MovieSet)movie_set.value);
  }
  // For every movie set, create a LLIter
  DestroyHashtableIterator(iter);
}

// Helper function to count number of items.
int countItems(MovieSet movie_set) {
  LLIter iter = CreateLLIter(movie_set->movies);
  int count = 1;
  while (LLIterHasNext(iter) == 1) {
    count++;
    LLIterNext(iter);
  }
  DestroyLLIter(iter);
  return count;
}

void OutputMovieSet(MovieSet movie_set) {
  LLIter iter = CreateLLIter(movie_set->movies);
  void* payload;
  int flag = 0;
  printf("\n%d items\n", countItems(movie_set));
  printf("IndexType: %s\n", movie_set->desc);
  while (flag == 0) {
    LLIterGetPayload(iter, &payload);
    printf("%s\n", ((Movie*)payload)->title);
    if (LLIterHasNext(iter) == 1) {
      LLIterNext(iter);
    } else { flag = 1; }
  }
  DestroyLLIter(iter);
}


