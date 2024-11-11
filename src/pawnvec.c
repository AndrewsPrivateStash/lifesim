

#include "../include/pawnvec.h"
#include "../include/pawn.h"
#include <stdio.h>
#include <stdlib.h>


static const int _REALLOC_SCALE_FACTOR = 2;          // factor scaling the resizing of an array
static const int _PAWNVEC_STARTING_CAPACITY = 10;    // Starting capacity of a new PawnVec


PawnVec *pawnvec_new(void) {
    PawnVec *pv = malloc(sizeof(PawnVec));
    if (!pv) return NULL;   // let caller check

    pv->ps = malloc(sizeof(struct MigPawn*) * _PAWNVEC_STARTING_CAPACITY);
    if (!pv->ps) {
        free(pv);
        return NULL;
    }

    pv->cap = _PAWNVEC_STARTING_CAPACITY;
    pv->len = 0;

    return pv;
}


void pawnvec_free(PawnVec *pv) {
    for (int i = 0; i< pv->len; i++) {
        free(pv->ps[i]);
    }
    free(pv->ps);
    free(pv);
}


void pawnvec_resize(PawnVec *pv) {
    struct MigPawn **tmp = realloc(pv->ps, sizeof(struct MigPawn*) * (pv->cap) * _REALLOC_SCALE_FACTOR);
    if (!tmp) {
        fprintf(stderr, "failed to reallocate pawns array\n");
        exit(1);
    }
    pv->cap *= _REALLOC_SCALE_FACTOR;
    pv->ps = tmp;
}


void pawnvec_add(PawnVec *pv, struct MigPawn *p) {
    if (pv->cap == pv->len) pawnvec_resize(pv);
    pv->ps[pv->len] = p;
    pv->len++;
}


struct MigPawn *pawnvec_new_migpawn(Pawn *p, Point2d pnt) {
    struct MigPawn *mp = malloc(sizeof(struct MigPawn));
    if (!mp) return NULL;   // caller checks

    mp->p = p;
    mp->mig_pnt = pnt;

    return mp;
}