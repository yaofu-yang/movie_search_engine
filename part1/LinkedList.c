// CS 5007, Northeastern University, Seattle
// Summer 2019
// Adrienne Slaughter
//
// Inspired by UW CSE 333; used with permission.
//
// This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
// It is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.


//////////////////////
///  Yao-Fu Yang  ////
///  03/10/2020   ////
//////////////////////

#include "LinkedList.h"
#include "LinkedList_priv.h"
#include "Assert007.h"

#include <stdio.h>
#include <stdlib.h>

LinkedList CreateLinkedList() {
  LinkedList list = (LinkedList)malloc(sizeof(LinkedListHead));
  if (list == NULL) {
    // out of memory
    return (LinkedList) NULL;
  }
  list->num_elements = 0;
  list->head = NULL;
  list->tail = NULL;

  return list;
}

int DestroyLinkedList(LinkedList list,
                      LLPayloadFreeFnPtr payload_free_function) {
  Assert007(list != NULL);
  Assert007(payload_free_function != NULL);

  // Case where LL is not empty.
  LinkedListNode *current = list->head;
  LinkedListNode *next;
  while (current != NULL) {
    next = current->next;
    payload_free_function(current->payload);
    DestroyLinkedListNode(current);
    current = next;
  }
  free(list);
  return 0;
}

unsigned int NumElementsInLinkedList(LinkedList list) {
  Assert007(list != NULL);
  return list->num_elements;
}

LinkedListNodePtr CreateLinkedListNode(void *data) {
    LinkedListNodePtr node = (LinkedListNodePtr)malloc(sizeof(LinkedListNode));
    if (node == NULL) {
        // Out of memory
        return NULL;
    }
    node->payload = data;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

int DestroyLinkedListNode(LinkedListNode *node) {
  Assert007(node != NULL);
  node->payload = NULL;
  node->next = NULL;
  node->prev = NULL;
  free(node);
  return 0;
}

int InsertLinkedList(LinkedList list, void *data) {
  Assert007(list != NULL);
  Assert007(data != NULL);
  LinkedListNodePtr new_node = CreateLinkedListNode(data);

  if (new_node == NULL) {
    return 1;
  }

  if (list->num_elements == 0) {
    Assert007(list->head == NULL);  // debugging aid
    Assert007(list->tail == NULL);  // debugging aid
    list->head = new_node;
    list->tail = new_node;
    new_node->next = new_node->prev = NULL;
    list->num_elements = 1U;
    return 0;
  }

  if (list->num_elements >= 1) {
    Assert007(list->head != NULL);
    Assert007(list->tail != NULL);
    new_node->prev = NULL;
    list->head->prev = new_node;
    new_node->next = list->head;
    list->head = new_node;
    list->num_elements++;
    return 0;
  }
  return 0;
}

int AppendLinkedList(LinkedList list, void *data) {
  Assert007(list != NULL);
  Assert007(data != NULL);
  LinkedListNodePtr new_node = CreateLinkedListNode(data);

  if (list->num_elements == 0) {
    Assert007(list->head == NULL);
    Assert007(list->tail == NULL);
    list->head = new_node;
    list->tail = new_node;
    new_node->next = new_node->prev = NULL;
    list->num_elements = 1U;
    return 0;
  }

  if (list->num_elements >= 1) {
    Assert007(list->head != NULL);
    Assert007(list->tail != NULL);
    new_node->next = NULL;
    new_node->prev = list->tail;
    list->tail->next = new_node;
    list->tail = new_node;
    list->num_elements++;
    return 0;
  }
  return 0;
}

int PopLinkedList(LinkedList list, void **data) {
  Assert007(list != NULL);
  Assert007(data != NULL);
  if (list == NULL) {
    return 1;
  }

  // Case with only one element.
  if (list->num_elements == 1) {
    *data = list->head->payload;
    LinkedListNode *temp_head = list->head;
    list->head = NULL;
    list->num_elements--;
    DestroyLinkedListNode(temp_head);
    return 0;
  }

  // Case with at least 2 elements.
  if (list->num_elements >= 2) {
    *data = list->head->payload;
    LinkedListNode *temp_head = list->head;
    list->head = list->head->next;
    list->head->prev = NULL;
    list->num_elements--;
    DestroyLinkedListNode(temp_head);
    return 0;
  }
  return 0;
}

int SliceLinkedList(LinkedList list, void **data) {
    Assert007(list != NULL);
    Assert007(data != NULL);

  if (list == NULL) {
    return 1;
  }

  // Case with only one element.
  if (list->num_elements == 1) {
    *data = list->head->payload;
    LinkedListNode *temp_head = list->head;
    list->head = NULL;
    list->num_elements--;
    DestroyLinkedListNode(temp_head);
    return 0;
  }

  // Case with at least 2 elements.
  if (list->num_elements >= 2) {
    *data = list->tail->payload;
    LinkedListNode *temp_tail = list->tail;
    list->tail = list->tail->prev;
    list->tail->next = NULL;
    list->num_elements--;
    DestroyLinkedListNode(temp_tail);
    return 0;
  }
  return 0;
}

void SortLinkedList(LinkedList list,
                    unsigned int ascending,
                    LLPayloadComparatorFnPtr compare) {
    Assert007(list != NULL);
    if (list->num_elements <2) {
        return;
    }

    int swapped;
    do {
      LinkedListNodePtr curnode = list->head;
      swapped = 0;

      while (curnode->next != NULL) {
        // compare this node with the next; swap if needed
        int compare_result = compare(curnode->payload, curnode->next->payload);

        if (ascending) {
          compare_result *= -1;
        }

        if (compare_result < 0) {
          // swap
          void* tmp;
          tmp = curnode->payload;
          curnode->payload = curnode->next->payload;
          curnode->next->payload = tmp;
          swapped = 1;
        }
        curnode = curnode->next;
      }
    } while (swapped);
}

void PrintLinkedList(LinkedList list) {
    printf("List has %lu elements. \n", list->num_elements);
}


LLIter CreateLLIter(LinkedList list) {
  Assert007(list != NULL);
  Assert007(list->num_elements > 0);

  LLIter iter = (LLIter)malloc(sizeof(struct ll_iter));
  Assert007(iter != NULL);

  iter->list = list;
  iter->cur_node = list->head;
  return iter;
}

int LLIterHasNext(LLIter iter) {
  Assert007(iter != NULL);
  return (iter->cur_node->next != NULL);
}

int LLIterNext(LLIter iter) {
  Assert007(iter != NULL);
  if (LLIterHasNext(iter)) {
    iter->cur_node = iter->cur_node->next;
    return 0;
  }
  return 1;
}

int LLIterGetPayload(LLIter iter, void** data) {
  Assert007(iter != NULL);
  *data = iter->cur_node->payload;
  return 0;
}


int LLIterHasPrev(LLIter iter) {
  Assert007(iter != NULL);
  return (iter->cur_node->prev != NULL);
}

int LLIterPrev(LLIter iter) {
  Assert007(iter != NULL);
  if (LLIterHasPrev(iter)) {
    iter->cur_node = iter->cur_node->prev;
    return 0;
  }
  return 1;
}

int DestroyLLIter(LLIter iter) {
  Assert007(iter != NULL);
  iter->cur_node = NULL;
  iter->list = NULL;
  free(iter);
  return 0;
}

int LLIterInsertBefore(LLIter iter, void* payload) {
  Assert007(iter != NULL);
  if ((iter->list->num_elements <= 1) ||
      (iter->cur_node == iter->list->head)) {
    // insert item
    return InsertLinkedList(iter->list, payload);
  }

  LinkedListNodePtr new_node = CreateLinkedListNode(payload);
  if (new_node == NULL) return 1;

  new_node->next = iter->cur_node;
  new_node->prev = iter->cur_node->prev;
  iter->cur_node->prev->next = new_node;
  iter->cur_node->prev = new_node;

  iter->list->num_elements++;

  return 0;
}

int LLIterDelete(LLIter iter, LLPayloadFreeFnPtr payload_free_function) {
  Assert007(iter != NULL);
  if (iter->list->num_elements == 1) {
    DestroyLinkedList(iter->list, payload_free_function);
    DestroyLLIter(iter);
    return 1;

  } else if (!LLIterHasPrev(iter)) {
    LinkedListNode *temp_node = iter->cur_node;
    iter->list->head = iter->list->head->next;
    iter->list->head->prev = NULL;
    iter->list->num_elements--;
    iter->cur_node = iter->list->head;

    payload_free_function(temp_node->payload);
    DestroyLinkedListNode(temp_node);
    return 0;

  } else if (!LLIterHasNext(iter)) {
    LinkedListNode *temp_node = iter->cur_node;
    iter->list->tail = iter->list->tail->prev;
    iter->list->tail->next = NULL;
    iter->list->num_elements--;
    iter->cur_node = iter->list->tail;

    payload_free_function(temp_node->payload);
    DestroyLinkedListNode(temp_node);
    return 0;

  } else {
    LinkedListNode *temp_node = iter->cur_node;
    iter->cur_node->prev->next = iter->cur_node->next;
    iter->cur_node->next->prev = iter->cur_node->prev;
    iter->list->num_elements--;
    iter->cur_node = iter->cur_node->next;

    payload_free_function(temp_node->payload);
    DestroyLinkedListNode(temp_node);
    return 0;
  }
}

