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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>


#include "gtest/gtest.h"

extern "C" {
  #include "./MovieIndex.h"
  #include "htll/LinkedList.h"
  #include "htll/Hashtable.h"
  #include "MovieIndex.h"
  #include "Movie.h"
  #include "MovieReport.h"
  #include "FileParser.h"
  #include <string.h>
}

const char* movie_row_A = "9.3|The Shawshank Redemption|R|Crime|142|Tim Robbins,Morgan Freeman,Bob Gunton";
const char* movie_row_B = "7.4|Back to the Future Part III|PG|Adventure|118|Michael J. Fox,Christopher Lloyd,Mary Steenburgen";

void DestroyLLMovie(void *payload) {
  DestroyMovie((Movie*)payload);
}

TEST(MovieSet, CreateDestroy) {
  MovieSet set = CreateMovieSet("My test set");
  ASSERT_NE(set, nullptr);
  DestroyMovieSet(set);
}

TEST(MovieSet, AddOneMovie) {
  MovieSet set = CreateMovieSet("My test set");
  ASSERT_NE(set, nullptr);
  ASSERT_EQ(NumElementsInLinkedList(set->movies), 0);

  char row[1000];
  strcpy(row, movie_row_A);
  MoviePtr movie = CreateMovieFromRow(row);
  int res = AddMovieToSet(set, movie);
  ASSERT_EQ(res, 0);
  ASSERT_EQ(NumElementsInLinkedList(set->movies), 1);

  DestroyMovieSet(set);
  DestroyMovie(movie);
}

TEST(MovieIndex, CreateDestroy) {
  Index index = CreateIndex();
  ASSERT_NE(index, nullptr);
  DestroyIndex(index);
}

// DONE (Yao-Fu): Adds helper method to check set contains movie.
// Helper method to see if a set contains the input movie.
int SetContainsMovie(Index index, Movie* movie, enum IndexField field) {
  HTKeyValue kvp;
  HTKeyValue old_kvp;
  kvp.key = ComputeKey(movie, field, 0);
  if (LookupInHashtable(index, kvp.key, &old_kvp) != 0) {
    return 1;
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
        return 0;
      }
      if (LLIterHasNext(iter) == 1) {
        LLIterNext(iter);
      } else { flag = 1; }
    }
    DestroyLLIter(iter);
    return 0;
  }
}

TEST(MovieIndex, AddMovieToIndex) {
  // Copying the string from a const to an array
  // (to be more similar to the actual use case)
  char row[1000];
  strcpy(row, movie_row_A);
  // Take a movie
  MoviePtr m1 = CreateMovieFromRow(row);
  strcpy(row, movie_row_B);
  MoviePtr m2 = CreateMovieFromRow(row);

  // Create Index
  Index index = CreateIndex();

  // Add movie to index
  AddMovieToIndex(index, m1, ContentRating);

  // DONE (Yao-Fu): Adds test to assess movie is found within set.
  // Check that movie is in index
  ASSERT_EQ(0, SetContainsMovie(index, m1, ContentRating));
  ASSERT_EQ(1, SetContainsMovie(index, m2, ContentRating));

  // Check size/num elements (should be num of types, not of movies)
  ASSERT_EQ(NumElemsInHashtable(index), 1);

  // Try to insert movie again
  AddMovieToIndex(index, m1, ContentRating);

  // Add another movie to the index (same IndexType)
  AddMovieToIndex(index, m2, ContentRating);

  // Destroy movie index
  DestroyIndex(index);

  // DONE (Yao-Fu) Added DestroyMovie since code now makes copies during
  // AddMovieToSet in order to avoid duplicate free() calls in the
  // example_indexer.c file.
  DestroyMovie(m1);
  DestroyMovie(m2);
}

// DONE (Yao-Fu): [Moved over from the example_indexer.c file.]
void DestroyNothing(void* thing) {
  // Helper method to destroy the LinkedList.
}


TEST(MovieIndex, BuildMovieIndexFromFile) {
  LinkedList movie_list  = ReadFile(const_cast<char *>("data/test"));

  ASSERT_EQ(5u, NumElementsInLinkedList(movie_list));

  Index index = BuildMovieIndex(movie_list, Genre);

  // DONE (Yao-Fu): Check number of elements in the index.
  ASSERT_EQ(NumElemsInHashtable(index), 2);

  // DONE (Yao-Fu): Added DestroyLinkedList to remove remaining memory leak.
  DestroyLinkedList(movie_list, &DestroyNothing);
  DestroyIndex(index);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
