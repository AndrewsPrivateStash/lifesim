/*
    list of tokens

*/

#ifndef CLIST_H_
#define CLIST_H_

#include <stdlib.h>
#include <stdint.h>
#include "config.h"


typedef struct ConfToken ConfToken;

// node struct
typedef struct Cnode {
    ConfToken *token;           // current token
    struct Cnode *next;         // next node 
} Cnode;

// list struct
typedef struct Clist {
    Cnode *head;
    Cnode *tail;
    uint16_t cnt;
} Clist;

// make new list
Clist *clist_new_list(void);
// free the list
void clist_free(Clist*);


// add node to front (new head)
void clist_push(Clist*, ConfToken);
// return copy of head node and remove from list
ConfToken clist_pop(Clist*);
// add new node to back
void clist_push_back(Clist*, ConfToken);
// return copy of last node and remove from list
ConfToken clist_pop_back(Clist*);

// print the elements of the list
void clist_print(Clist*, FILE*);


#endif
