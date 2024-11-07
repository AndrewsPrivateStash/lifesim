/*
    defines the pawn entity which is the protagonist in our little simulation
    pawns have no gender for simplicity

*/


#ifndef PAWN_H_
#define PAWN_H_

#include <stdbool.h>
#include "plist.h"

typedef struct Plist Plist;     // forward decleration

typedef struct Pawn {
    unsigned int id;                    // unique pawn id
    int x_pos, y_pos;                   // x and y location on grid (top left is (0,0))
    bool alive;                         // is the pawn alive
    unsigned int bday;                  // season of birth
    unsigned short age;                 // age of pawn
    unsigned short gen_age;             // random genetic death age
    bool fertile;                       // is the pawn able to have ofspring
    unsigned short fertility_factor;    // factor affecting mating chance (decreases with age)
    unsigned short mating_radius;       // genetic pixel radius for mating
    unsigned short mating_factor;       // genetic attractiveness (chance an available pawn will mate)
    bool mated;                         // did this pawn mate this season; reset after mating period
    Plist *possible_mates;              // list of mates in radius

} Pawn;

Pawn *pawn_new(unsigned int id, int x, int y, unsigned int bday, bool pre_age);
void pawn_age(Pawn*);
void pawn_print(Pawn*);
void pawn_free(Pawn*);


#endif