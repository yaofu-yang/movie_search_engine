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
#include <sys/stat.h>
#include <dirent.h>

#include "FileCrawler.h"
#include "DocIdMap.h"
#include "LinkedList.h"

#define kMaxFileNameLen 30

/////////////////
// Yao-Fu Yang //
// 03/29/2020  //
/////////////////

void CrawlFilesToMap(const char *dir, DocIdMap map) {
  struct stat s;
  struct dirent **namelist;
  int n;
  int length;
  n = scandir(dir, &namelist, 0, alphasort);

  // Portion dealing with iterating through the namelist.
  for (int i = 0; i < n; i++) {
    char *name = namelist[i]->d_name;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
      free(namelist[i]);  // Do not need these.
      continue;
    }

    char *filepath = (char*)malloc(kMaxFileNameLen * sizeof(filepath));
    length = strlen(dir) + strlen(name) + 1;
    snprintf(filepath, length, "%s%s", dir, name);

    if (stat((const char*)filepath, &s) == 0 && S_ISREG(s.st_mode)) {
      // File was regular
      PutFileInMap(filepath, map);
    } else {
      // Not a regular file.
      char *new_path = (char*)malloc(kMaxFileNameLen * sizeof(new_path));
      snprintf(new_path, length + 1, "%s%s", filepath, "/");
      free(filepath);
      // Check all sub-directories recursively.
      CrawlFilesToMap((const char*)new_path, map);
      free(new_path);
    }
    free(namelist[i]);
  }
  free(namelist);
}
