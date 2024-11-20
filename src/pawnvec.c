

#include "../include/pawnvec.h"
#include "../include/pawn.h"
#include <stdio.h>
#include <stdlib.h>


static const int _REALLOC_SCALE_FACTOR = 2;          // factor scaling the resizing of an array
static const int _PAWNVEC_STARTING_CAPACITY = 10;    // Starting capacity of a new PawnVec



// ############## PawnVec ##############

PawnVec *pawnvec_new(void) {
    PawnVec *pv = malloc(sizeof(PawnVec));
    if (!pv) return NULL;   // let caller check

    pv->ps = malloc(sizeof(Pawn*) * _PAWNVEC_STARTING_CAPACITY);
    if (!pv->ps) {
        free(pv);
        return NULL;
    }

    pv->cap = _PAWNVEC_STARTING_CAPACITY;
    pv->len = 0;

    return pv;
}


void pawnvec_free(PawnVec *pv) {
    // does not free pawns pointed to
    free(pv->ps);
    pv->ps = NULL;
    free(pv);
    pv = NULL;
}


void pawnvec_free_deep(PawnVec *pv) {
    // free the pawns pointed to
    for (int i = 0; i<pv->len; i++) {
        pawn_free(pv->ps[i]);
    }
    free(pv->ps);
    pv->ps = NULL;
    free(pv);
    pv = NULL;
}


void pawnvec_resize(PawnVec *pv) {
    // vec and pawn pointers preserved
    if (pv->len < pv->cap) return;  // didn't need to be called
    Pawn **tmp = realloc(pv->ps, sizeof(Pawn*) * (pv->cap) * _REALLOC_SCALE_FACTOR);
    if (!tmp) {
        fprintf(stderr, "failed to resize pawns array\n");
        exit(1);
    }
    pv->cap *= _REALLOC_SCALE_FACTOR;
    pv->ps = tmp;
}


void pawnvec_add(PawnVec *pv, Pawn *p) {
    if (!p) return;
    if (pv->cap == pv->len) pawnvec_resize(pv);
    pv->ps[pv->len] = p;
    pv->len++;
}


void pawnvec_print(PawnVec *pv) {
    printf("PawnVec: %p, len: %d, cap: %d\n", pv, pv->len, pv->cap);
    for (int i = 0; i<pv->len; i++) {
        printf("id:%d, Pawn:%u, CurLoc:(%d, %d), alive: %s\n",
            i,
            pv->ps[i]->id,
            pv->ps[i]->x_pos,
            pv->ps[i]->y_pos,
            pv->ps[i]->alive ? "TRUE": "FALSE"
        );
    }
}


void pawnvec_print_to_file(FILE *f, PawnVec *pv, const char *header) {
    Pawn *p=NULL;
    fprintf(f, "%s\n", header);
    if(!pv) return;
    if (pv->len == 0) return;
    for (int i = 0; i<pv->len; i++) {
        p = pv->ps[i];
        fprintf(f, "id:%u, age:%u, alive:%d, bday:%u, (%d,%d), gen_age:%u\n",
            p->id,
            p->age,
            p->alive,
            p->bday,
            p->x_pos,
            p->y_pos,
            p->gen_age
        );
    }
}



// ############## MigVec ##############

MigVec *pawnvec_new_mg(void) {
    MigVec *pv = malloc(sizeof(MigVec));
    if (!pv) return NULL;   // let caller check

    pv->ps = malloc(sizeof(MigPawn*) * _PAWNVEC_STARTING_CAPACITY);
    if (!pv->ps) {
        free(pv);
        return NULL;
    }

    pv->cap = _PAWNVEC_STARTING_CAPACITY;
    pv->len = 0;

    return pv;
}


void pawnvec_free_mg(MigVec *pv) {
    for (int i = 0; i< pv->len; i++) {
        free(pv->ps[i]);
    }
    free(pv->ps);
    free(pv);
}


void pawnvec_resize_mg(MigVec *pv) {
    if (pv->len < pv->cap) return;  // doens't need to be resized
    MigPawn **tmp = realloc(pv->ps, sizeof(MigPawn*) * (pv->cap) * _REALLOC_SCALE_FACTOR);
    if (!tmp) {
        fprintf(stderr, "failed to resize pawns array\n");
        exit(1);
    }
    pv->cap *= _REALLOC_SCALE_FACTOR;
    pv->ps = tmp;
}


void pawnvec_add_mg(MigVec *pv, MigPawn *p) {
    if (pv->cap == pv->len) pawnvec_resize_mg(pv);
    pv->ps[pv->len] = p;
    pv->len++;
}


MigPawn *pawnvec_new_migpawn(Pawn *p, Point2d pnt) {
    MigPawn *mp = malloc(sizeof(MigPawn));
    if (!mp) return NULL;   // caller checks

    mp->p = p;
    mp->mig_pnt = pnt;

    return mp;
}


void pawnvec_print_mg(MigVec *mg) {
    printf("MigVec: %p, len: %d, cap: %d\n", mg, mg->len, mg->cap);
    for (int i = 0; i<mg->len; i++) {
        printf("id:%d, Pawn:%u, CurLoc:(%d, %d), MigLoc:(%d, %d)\n",
            i,
            mg->ps[i]->p->id,
            mg->ps[i]->p->x_pos,
            mg->ps[i]->p->y_pos,
            mg->ps[i]->mig_pnt.x,
            mg->ps[i]->mig_pnt.y
        );
    }
}
