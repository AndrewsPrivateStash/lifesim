/*
    list of pawns, where list holds pawns inside of mating radius for the owning pawn
    and the each node holds the distance to owning pawn
*/

#ifndef PLIST_H_
#define PLIST_H_

#include <stdlib.h>
#include "pawn.h"

typedef struct Pawn Pawn;   //forward dec

// node struct
typedef struct Pnode {
    Pawn *pwn;              // current pawn
    unsigned short dist;    // distance to owning pawn
    struct Pnode *next;     // next node
} Pnode;

// list struct
typedef struct Plist {
    Pnode *head;
    Pawn *owning_pawn;
    int cnt;
} Plist;

// adds node to the list
void plist_add_node(Plist*, Pawn*, unsigned short);
// removes node from list
void plist_remove_node(Plist*, Pawn*);
// does pawn exist in list
bool plist_inlist(Plist*, Pawn*);
// free the list
void plist_free(Plist*);
// purge the dead
void plist_purge(Plist*);
// print the elements of the list
void plist_print(Plist*);


#endif
