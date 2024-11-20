/*
    PawnVec is a vector structure to easily store dynamic lists of pawns
*/


#ifndef PAWN_VEC_H_
#define PAWN_VEC_H_

#include <stdio.h>
#include "pawn.h"
#include "util.h"

typedef struct PawnVec {
    Pawn **ps;    
    int cap;                // array capacity
    int len;                // array length
} PawnVec;


typedef struct MigPawn {
    Pawn *p;            // pawn
    Point2d mig_pnt;    // migration point
} MigPawn;


typedef struct MigVec {
    struct MigPawn **ps;    
    int cap;                // array capacity
    int len;                // array length
} MigVec;




// make vector
PawnVec *pawnvec_new(void);
MigVec *pawnvec_new_mg(void);

// resize pawnvec
void pawnvec_resize(PawnVec*);
void pawnvec_resize_mg(MigVec*);

// add value
void pawnvec_add(PawnVec*, Pawn*);
void pawnvec_add_mg(MigVec*, struct MigPawn*);

// free the vector
void pawnvec_free(PawnVec*);
void pawnvec_free_deep(PawnVec*);
void pawnvec_free_mg(MigVec*);

// make a new MigPawn
struct MigPawn *pawnvec_new_migpawn(Pawn *p, Point2d pnt);

//print a Vector
void pawnvec_print(PawnVec*);
void pawnvec_print_to_file(FILE *f, PawnVec *pv, const char *header);
void pawnvec_print_mg(MigVec*);


#endif