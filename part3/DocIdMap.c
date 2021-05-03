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
#include "DocIdMap.h"
#include "Hashtable.h"

/////////////////
// Yao-Fu Yang //
// 03/29/2020  //
/////////////////

void DestroyString(void *val) {
    free(val);
}

DocIdMap CreateDocIdMap() {
  DocIdMap docs = (DocIdMap)CreateHashtable(64);
  return docs;
}

void DestroyDocIdMap(DocIdMap map) {
  DestroyHashtable(map, &DestroyString);
}

int PutFileInMap(char *filename, DocIdMap map) {
  HTKeyValue kvp;
  HTKeyValue old_kvp;
  kvp.key = NumElemsInHashtable(map) + 1;
  kvp.value = filename;
  int result = PutInHashtable(map, kvp, &old_kvp);
  if (result == 1) {
    printf("No more memory\n");
    return 1;
  } else if (result == 2) {
    // Key already exists.

    if (kvp.value != old_kvp.value) {
      // Mismatched key-value pair; should never reach here.
      return 1;
    } else {
      // Has duplicate key-value pair.
      return 1;
    }
  }
  // Added successfully.
  return 0;
}

DocIdIter CreateDocIdIterator(DocIdMap map) {
  HTIter iter = CreateHashtableIterator(map);
  return iter;
}

void DestroyDocIdIterator(DocIdIter iter) {
  DestroyHashtableIterator(iter);
}

char *GetFileFromId(DocIdMap docs, int docId) {
  HTKeyValue kvp;
  if (LookupInHashtable(docs, docId, &kvp) == 0) {
    return kvp.value;
  } else {
    return NULL;
  }
}
