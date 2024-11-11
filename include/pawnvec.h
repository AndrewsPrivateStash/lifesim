/*
    PawnVec is a vector structure to easily store dynamic lists of pawns
*/


#ifndef PAWN_VEC_H_
#define PAWN_VEC_H_

#include "pawn.h"
#include "util.h"


struct MigPawn {
    Pawn *p;            // pawn
    Point2d mig_pnt;    // migration point
};


typedef struct PawnVec {
    struct MigPawn **ps;    
    int cap;                // array capacity
    int len;                // array length
} PawnVec;


// make vector
PawnVec *pawnvec_new(void);

// resize pawnvec
void pawnvec_resize(PawnVec*);

// add value
void pawnvec_add(PawnVec*, struct MigPawn*);

// free the vector
void pawnvec_free(PawnVec*);

// make a new MigPawn
struct MigPawn *pawnvec_new_migpawn(Pawn *p, Point2d pnt);


#endif