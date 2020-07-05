#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"



struct node{
    node_t *next;
    int data;
};

struct list{
    node_t *head;
    node_t *tail;
    int length;
};

list_t *new_list(){
    list_t *listnew = (list_t* )malloc(sizeof(list_t));
    listnew->head = NULL;
    listnew->tail = NULL;
    listnew->length = 0;
    return listnew;
}
node_t *new_node(int data, node_t *next){
    node_t *nodenew = (node_t* )malloc(sizeof(node_t));
    nodenew->data = data;
    nodenew->next = next;
    return nodenew;
}

void free_list(list_t *list){

    free(list);
}

void length_list(list_t *list){
    int len =0;
    node_t *next = list->head;
    while(next!=NULL){
        next = next->next;
        len++;
    }
    printf("Length of the list is: %d\n", len);
}

void add_front(list_t *list, int data){
    node_t *nodenew  = new_node( data, list->head);
    list->head = nodenew;
}

void add_end(list_t *list, int data){
    node_t *next = list->head;
    while(next->next!=NULL){
        next = next->next;
    }
    next->next = new_node( data, NULL);
}

void delete_front(list_t *list){
    node_t *temp;
    temp = list->head;
    list->head = list->head->next;
    free(temp);
}

void delete_end(list_t *list){
node_t *next = list->head, *temp, *temp2;
    while(next!=NULL){
        temp2 = temp;
        temp = next;
        next = next->next;
    }
    list->tail = temp;
    free(temp);
}
