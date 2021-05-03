/*
 *  Created by Adrienne Slaughter
 *  CS 5007 Summer 2019
 *  Northeastern University, Seattle
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "QueryProcessor.h"
#include "MovieTitleIndex.h"
#include "LinkedList.h"
#include "Hashtable.h"
#include "DocSet.h"

/////////////////
// Yao-Fu Yang //
// 03/29/2020  //
/////////////////

SearchResultIter CreateSearchResultIter(DocumentSet set) {
  SearchResultIter iter =
    (SearchResultIter)malloc(sizeof(struct searchResultIter));
  iter->doc_iter = CreateHashtableIterator(set->doc_index);

  HTKeyValue dest;
  HTIteratorGet(iter->doc_iter, &dest);
  iter->offset_iter = CreateLLIter(dest.value);
  iter->cur_doc_id = dest.key;
  return iter;
}

void DestroySearchResultIter(SearchResultIter iter) {
  // Destroy LLIter
  if (iter->offset_iter != NULL) {
    DestroyLLIter(iter->offset_iter);
  }
  // Destroy doc_iter
  DestroyHashtableIterator(iter->doc_iter);
  free(iter);
}


SearchResultIter FindMovies(MovieTitleIndex index, char *term) {
  DocumentSet set = GetDocumentSet(index, term);
  if (set == NULL) {
    return NULL;
  }
  printf("Getting docs for movieset term: \"%s\"\n", set->desc);
  SearchResultIter iter = CreateSearchResultIter(set);
  return iter;
}


int SearchResultGet(SearchResultIter iter, SearchResult output) {
  void* payload;
  LLIterGetPayload(iter->offset_iter, &payload);
  output->doc_id = iter->cur_doc_id;
  output->row_id = *(int*)payload;
  return 0;
}


int SearchResultNext(SearchResultIter iter) {
  if (SearchResultIterHasMore(iter) == 0) {
     // There are no more files.
    return -1;
  } else if (LLIterHasNext(iter->offset_iter) == 0) {
    // Reached the end of the linkedlist. Go to next one.
    HTIteratorNext(iter->doc_iter);
    DestroyLLIter(iter->offset_iter);

    // Populate new linked list and doc_id.
    HTKeyValue dest;
    HTIteratorGet(iter->doc_iter, &dest);
    iter->offset_iter = CreateLLIter(dest.value);
    iter->cur_doc_id = dest.key;
  } else {
      LLIterNext(iter->offset_iter);
  }
  return 0;
}


// Returns 0 if no more
int SearchResultIterHasMore(SearchResultIter iter) {
  if (iter->doc_iter == NULL) {
    return 0;
  }
  if (LLIterHasNext(iter->offset_iter) == 0) {
    return (HTIteratorHasMore(iter->doc_iter));
  }
  return 1;
}
