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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DocSet.h"
#include "Hashtable.h"
#include "Util.h"

/////////////////
// Yao-Fu Yang //
// 03/29/2020  //
/////////////////

int AddDocInfoToSet(DocumentSet set,  uint64_t docId, int rowId) {
  Hashtable doc_index = set->doc_index;
  HTKeyValue kvp;
  HTKeyValue old_kvp;
  int *id = (int*)malloc(sizeof(id));
  *id = rowId;
  kvp.key = docId;

  if (LookupInHashtable(doc_index, kvp.key, &old_kvp) != 0) {
    // Index does not contain key. Put new pair in index.
    LinkedList new_list = CreateLinkedList();
    kvp.value = new_list;
    PutInHashtable(doc_index, kvp, &old_kvp);
  } else {
    // Key already exists; need to check if RowID already exists.
    LinkedList row_ids = (LinkedList)old_kvp.value;
    LLIter iter = CreateLLIter(row_ids);
    void* payload;
    int flag = 0;
    // Using a flag to void redundant code in fence-post scenario.
    while (flag == 0) {
      LLIterGetPayload(iter, (void**)&payload);
      int cur_rowId = *(int*)payload;
      if (cur_rowId == rowId) {
        // Found an existing rowID
        DestroyLLIter(iter);
        free(id);
        return -1;
      }
      if (LLIterHasNext(iter) == 1) {
        LLIterNext(iter);
      } else { flag = 1; }
    }
    // Index contains the docId but not the rowID
    kvp.value = old_kvp.value;
    DestroyLLIter(iter);
  }
  // Done if there are no duplicate id-value pairs.
  InsertLinkedList(kvp.value, (void*)id);
  return 0;
}

int DocumentSetContainsDoc(DocumentSet set, uint64_t docId) {
  Hashtable doc_index = set->doc_index;
  HTKeyValue kvp;
  HTKeyValue old_kpv;
  kvp.key = docId;
  return LookupInHashtable(doc_index, kvp.key, &old_kpv);
}

void PrintOffsetList(LinkedList list) {
  printf("Printing offset list\n");
  LLIter iter = CreateLLIter(list);
  int* payload;
  while (LLIterHasNext(iter) != 0) {
    LLIterGetPayload(iter, (void**)&payload);
    printf("%d\t", *((int*)payload));
    LLIterNext(iter);
  }
  DestroyLLIter(iter);
}


DocumentSet CreateDocumentSet(char *desc) {
  DocumentSet set = (DocumentSet)malloc(sizeof(struct docSet));
  if (set == NULL) {
    // Out of memory
    printf("Couldn't malloc for movieSet %s\n", desc);
    return NULL;
  }
  int len = strlen(desc);
  set->desc = (char*)malloc((len + 1) *  sizeof(set->desc));
  if (set->desc == NULL) {
    printf("Couldn't malloc for movieSet->desc");
    return NULL;
  }
  snprintf(set->desc, len + 1, "%s", desc);
  set->doc_index = CreateHashtable(16);
  return set;
}

void DestroyOffsetList(void *val) {
  LinkedList list = (LinkedList)val;
  DestroyLinkedList(list, &SimpleFree);
}

void DestroyDocumentSet(DocumentSet set) {
  // Free desc
  free(set->desc);
  // Free doc_index
  DestroyHashtable(set->doc_index, &DestroyOffsetList);
  // Free set
  free(set);
}
