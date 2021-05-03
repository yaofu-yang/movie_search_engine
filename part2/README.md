- External resources (Cite them below)
  - https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
  - https://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm
  - https://www.geeksforgeeks.org/enumeration-enum-c/
  - https://www.programiz.com/c-programming/c-switch-case-statement
  - https://stackoverflow.com/questions/33405248/in-switch-statement-error-is-switch-quantity-not-an-integer
  - https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
  - Probably some more links that I cannot retrieve.

# A8: Building a Data Indexer

## Files: 
* ```FileParser```: Responsible for reading in a file, checking for errors, and parse out movies.
* ```Movie```: A struct holding all the Movie info (id, title, type, genre, etc.)
* ```MovieIndex```: A hashtable that indexes movies according to specified field. Basically wraps a hashtable with functionality specific to adding/removing/destroying with MovieSets. The key is the description for the document set, and the value is the document set.
* ```MovieReport```: Responsible for printing out a report given a MovieIndex, to the command line or file.
* ```MovieSet```: A set of Movies that are connected somehow: they have the same value for a given field. If the field is Year, all the movies in the set are made in the same year. It consists of a description and a LinkedList of Movies.


## To run a single Google test

Instead of running all the tests at once, you can run just a single test something like this: 

```
./test_fileparser --gtest_filter=FileParser.CreateMovieFromRow
```

Here, only the ```FileParser.CreateMovieFromRow``` test will run. 
