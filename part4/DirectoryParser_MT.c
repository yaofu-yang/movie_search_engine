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
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#include "DirectoryParser.h"
#include "MovieTitleIndex.h"
#include "Movie.h"
#include "DocIdMap.h"


#define kBufferSize 1000

//=======================
// To minimize the number of files we have, I'm
// putting the private function prototypes for
// the DirectoryParser here.


/**
 * Helper function to index a single file. 
 *
 * \return a pointer to the number of records (lines) indexed from the file
 */
void* IndexAFile_MT(void *toBeIter);

pthread_mutex_t ITER_MUTEX = PTHREAD_MUTEX_INITIALIZER;  // global variable
pthread_mutex_t INDEX_MUTEX = PTHREAD_MUTEX_INITIALIZER;  // global variable

// THINK: Why is this global?
MovieTitleIndex movieIndex;

// Handles the variable number of threads case.
pthread_t tid;

int ParseTheFiles_MT(DocIdMap docs, MovieTitleIndex index, int num_threads) {
  // Creating array to indicate number of threads to spawn.
  pthread_t tid[num_threads];
  clock_t start, end;
  double cpu_time_used;
  start = clock();

  // Setting global variable equal to the MovieTitleIndex being passed.
  movieIndex = index;

  // Creating Iterator for DocId to be passed into function.
  DocIdIter doc_id_iter = CreateDocIdIterator(docs);

  // Spwaning the threads to run the IndexAFile_MT.
  int i;
  for (i = 0; i < num_threads; i++) {
    pthread_create(&(tid[i]), NULL, &IndexAFile_MT, doc_id_iter);
  }

  // Preparation to capture the number of records accessed.
  int* counter = (int*)malloc(sizeof(counter));
  *counter = 0;
  int* result;

  // Joining all of the active threads.
  int j;
  for (j = 0; j < num_threads; j++) {
    pthread_join(tid[j], (void *)&result);
    *counter += *result;
    free(result);
  }
  int num_records = *counter;
  free(counter);

  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

  printf("Took %f seconds to execute. \n", cpu_time_used);

  // Finally done with Iterator.
  DestroyHashtableIterator(doc_id_iter);
  return num_records;
}

// Returns the number of records indexed.
// Adapted from DirectoryParser.c in a10 to include mutex.
int IndexAFileHelper(char *file, uint64_t doc_id, MovieTitleIndex index) {
  FILE *cfPtr;

  if ((cfPtr = fopen(file, "r")) == NULL) {
    printf("File could not be opened\n");
    return 0;
  } else {
    char buffer[kBufferSize];
    int row = 0;

    while (fgets(buffer, kBufferSize, cfPtr) != NULL) {
      Movie *movie = CreateMovieFromRow(buffer);
      pthread_mutex_lock(&INDEX_MUTEX);  // Lock access to index.
      int result = AddMovieTitleToIndex(index, movie, doc_id, row);
      pthread_mutex_unlock(&INDEX_MUTEX);  // Unlock access to index.
      if (result < 0) {
        fprintf(stderr, "Didn't add MovieToIndex.\n");
      }
      row++;
      DestroyMovie(movie);  // Done with this now
    }
    fclose(cfPtr);
    return row;
  }
}

// The actual function getting the filename and doing the indexing.
void* IndexAFile_MT(void *docname_iter) {
  DocIdIter iter = (DocIdIter)docname_iter;
  HTKeyValue dest;
  int flag = 0;
  int* records = (int*)malloc(sizeof(records));
  *records = 0;
  while (flag == 0) {
    pthread_mutex_lock(&ITER_MUTEX);  // lock access to iterator.
    if (HTIteratorGet(iter, &dest) == 0) {
      // Successful retrieval of data.
      pthread_mutex_unlock(&ITER_MUTEX);  // Unlock access to iterator.
      *records += IndexAFileHelper((char *)dest.value, dest.key, movieIndex);
    } else {
      printf("Did not retrieve data");
      pthread_mutex_unlock(&ITER_MUTEX);  // Unlock in else branch.
    }
    pthread_mutex_lock(&ITER_MUTEX);  // Lock access to iterator.
    if (HTIteratorHasMore(iter) != 0) {
      // Goes to next if there are any.
      HTIteratorNext(iter);
    } else { flag = 1; }
    pthread_mutex_unlock(&ITER_MUTEX);  // Unlock access to iterator.
  }
  return (void*)records;
}

