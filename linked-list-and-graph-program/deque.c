/* * * * * * *
 * Deque module (i.e., double ended queue) for Assignment 1
 *
 * created for COMP20007 Design of Algorithms 2020
 * template by Tobias Edwards <tobias.edwards@unimelb.edu.au>
 * implementation by <Insert Name Here>
 */

// You must not change any of the code already provided in this file, such as
// type definitions, constants or functions.
//
// You may, however, add additional functions and/or types which you may need
// while implementing your algorithms and data structures.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "deque.h"
#include "util.h"

// Create a new empty Deque and return a pointer to it
Deque *new_deque() {
	Deque *deque = malloc(sizeof(*deque));
	assert(deque);

	deque->top = NULL;
	deque->bottom = NULL;
	deque->size = 0;

	return deque;
}

// Free the memory associated with a Deque
void free_deque(Deque *deque) {
  // Remove (and thus free) all of the nodes in the Deque.
  while (deque->size > 0) {
    deque_remove(deque);
  }

	// Free the deque struct itself
	free(deque);
}

// Create a new Node with a given piece of data
Node *new_node(Data data) {
  Node *node = malloc(sizeof(*node));
  assert(node);

  node->next = NULL;
  node->prev = NULL;
  node->data = data;

  return node;
}

// Free the memory associated with a Node
void free_node(Node *node) {
  free(node);
}

// Add an element to the top of a Deque
void deque_push(Deque *deque, Data data) {
  Node *new = new_node(data);

  if (deque->size > 0) {
    new->next = deque->top;
    deque->top->prev = new;
  } else {
    // If the Deque was initially empty then new is both the top and bottom
    deque->bottom = new;
  }

  deque->top = new;
  deque->size++;
}

// Add an element to the bottom of a Deque
void deque_insert(Deque *deque, Data data) {
  Node *new = new_node(data);

  if (deque->size > 0) {
    new->prev = deque->bottom;
    deque->bottom->next = new;
  } else {
    // If the Deque was initially empty then new is both the top and bottom
    deque->top = new;
  }

  deque->bottom = new;
  deque->size++;
}

// Remove and return the top element from a Deque
Data deque_pop(Deque *deque) {
  if (deque->size == 0) {
    exit_with_error("can't pop from empty Deque");
  }

  Data data = deque->top->data;
  Node *old_top = deque->top;

  if (deque->size == 1) {
    deque->top = NULL;
    deque->bottom = NULL;
  } else {
    deque->top = old_top->next;
    deque->top->prev = NULL;
  }

  deque->size--;

  free(old_top);

  return data;
}

// Remove and return the bottom element from a Deque
Data deque_remove(Deque *deque) {
  if (deque->size == 0) {
    exit_with_error("can't remove from empty Deque");
  }

  Data data = deque->bottom->data;
  Node *old_bottom = deque->bottom;

  if (deque->size == 1) {
    deque->top = NULL;
    deque->bottom = NULL;
  } else {
    deque->bottom = old_bottom->prev;
    deque->bottom->next = NULL;
  }

  deque->size--;

  free(old_bottom);

  return data;
}

// Return the number of elements in a Deque
int deque_size(Deque *deque) {
  return deque->size;
}

// Print the Deque on its own line with the following format:
//   [x_1, x_2, ..., x_n]
//     ^              ^
//    top           bottom
void print_deque(Deque *deque) {
  Node *current = deque->top;
  int i = 0;

  printf("[");

  while (current) {
    printf("%d", current->data);
    // Print a comma unless we just printed the final element
    if (i < deque->size - 1) {
      printf(", ");
    }
    current = current->next;
    i++;
  }

  printf("]\n");
}

// Reverse the Deque using an iterative approach
void iterative_reverse(Deque *deque) {
  int i;
  Node *current = deque->top; 
  for(i=0; i<deque_size(deque);i++){
    swap_nodes(&current->next,&current->prev);
    current = current->prev;
  }
  swap_nodes(&deque->top, &deque->bottom);
  free(current);
}

// Reverse the Deque using a recursive approach
void recursive_reverse(Deque *deque) {
  Node *current = deque->top;
  swap_nodes(&current->next,&current->prev);
  if(deque->bottom==deque->top){
    Node *temp=deque->bottom, *prev = NULL;
    while(temp){
      prev = temp;
      temp = temp->next;
    }
    deque->bottom = prev;
    return;
  }
  deque->top = current->prev;
  recursive_reverse(deque);
}

// Split the Deque given a critical value k, such that the Deque contains
// all elements greater than equal to k above (i.e., closer to the top)
// the elements less than k.
//
// Within the two parts of the array (>= k and < k) the elements should
// be in their original order.
//
// This function must run in linear time.
void split_deque(Deque *deque, int k) {
  if(!deque_size(deque)){
    return;
  }
  int i;
  //upper
  Node *current = deque->top, *upper=NULL, *lower=NULL;
  deque->top=NULL;
  deque->bottom=NULL;
  for(i=0; i<deque_size(deque);i++){
    if(current->data>=k){
      if(!deque->top){
        deque->top = current;
      }
      else{
          upper->next=current;
          current->prev=upper;
      }
      upper = current;
    }
    else{
      if(!lower){
        lower = current;
      }
      else{
          deque->bottom->next=current;
          current->prev=deque->bottom;
      }
      deque->bottom = current;
    }
    current = current->next;
  }
  if(!deque->top){
    deque->top = lower;
  }
  if(!lower){
    deque->bottom = upper;
  }
  if(lower&&upper){
    lower->prev=upper;
    upper->next=lower;
  }  
  deque->top->prev = NULL;
  deque->bottom->next = NULL;
//exit_with_error("split_deque not implemented");
}

void swap_nodes(Node** node1, Node** node2){
  Node *temp = *node1;
  *node1=*node2;
  *node2=temp;
}
