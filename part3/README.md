- External Resources
  - https://stackoverflow.com/questions/18402428/how-to-properly-use-scandir-in-c
  - https://stackoverflow.com/questions/3828192/checking-if-a-directory-exists-in-unix-system-call
  - https://linux.die.net/man/3/stat
  - https://www.thegeekstuff.com/2012/06/c-directory/
  - https://www.gnu.org/software/libc/manual/html_node/Scanning-Directory-Content.html
  - https://www.geeksforgeeks.org/strcat-vs-strncat-c/

# A9: Indexing Files to Query

## Files: 
* ```DocIDMap```: A Hashmap that contains a Document ID (docid) that maps it to the string representing the file pathway. Given a docid, we can then retrieve the filename and subsequently process it for information.

* ```FileCrawler```: Responsible for iterating through a given directory (and sub-directories), and for all files it finds, adds them to the provided DocIdMap to assign a unique ID.

* ```DocSet```: Essentially represents a set of Ids to given movies. It's a struct that contains a char * as one field (name of the set) and a Hashtable as its second field. The Hashtable has keys (doc_Id) to find the file of interest and values (linkedlist of ints) that indicate which row the movie is in the file.

* ```DirectoryParser```: Given a map of all the files that we want to index and search, open each file and index the contents to a MovieTitleIndex.

* ```MovieTitleIndex```: A Hashtable where the key is a (search) term, and the value is either a MovieSet or SetofMovies. Supports indexing by one of multiple fields.

* ```QueryProcessor```: Responsible for processing the results of a search query by creating an iterator to search and pull out the movies matching each query.


* ```main```: Given a directory as a command line argument, parses the files accordingly and accepts single-word search queries from the user until they enter 'q' for 'quit.'


## To run the tests
 - ./test_suite

## To run main
 - ./main dir/

