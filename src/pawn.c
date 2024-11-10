
#include "../include/pawn.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// ############### Constants ###############
const int _GEN_AGE_LOWER_BND = 1;           // min genetic age of Pawn
const int _GEN_AGE_UPPER_BND = 80;          // max genetic age of Pawn
const int _FERT_FACT_LOWER_BND = 0;         // min fertility factor of Pawn
const int _FERT_FACT_UPPER_BND = 100;       // max fertility factor of Pawn
const int _MATING_RADIUS_LOWER_BND = 1;     // min mating radius of Pawn
const int _MATING_RADIUS_UPPER_BND = 20;    // max mating radius of Pawn
const int _MATING_FACTOR_LOWER_BND = 0;     // min mating factor of Pawn
const int _MATING_FACTOR_UPPER_BND = 100;   // max mating factor of Pawn

const double _FERTILITY_DECAY = 0.95;       // the decay factor applied to fertility factor each season
const int _FERTILITY_START = 13;            // the age a Pawn becomes fertile


// ############### Pawn functions ############### 

Pawn *pawn_new(unsigned int id, int x, int y, unsigned int bday, bool pre_age){
    Pawn *p = malloc(sizeof(Pawn)); // caller checks NULL
    if (p) {
        p->id = id;
        p->alive = true;
        p->bday = bday;
        p->x_pos = x;
        p->y_pos = y;
        p->age = 0;
        p->fertile = false;
        p->mated = false;
        
        // random values (set once; genetics from birth)
        p->gen_age = (unsigned short)GetRandomValue(_GEN_AGE_LOWER_BND,_GEN_AGE_UPPER_BND);
        p->fertility_factor = (unsigned short)GetRandomValue(_FERT_FACT_LOWER_BND,_FERT_FACT_UPPER_BND);    // decreases with age, dormant until _FERTILITY_START
        p->mating_radius = (unsigned short)GetRandomValue(_MATING_RADIUS_LOWER_BND,_MATING_RADIUS_UPPER_BND);
        p->mating_factor = (unsigned short)GetRandomValue(_MATING_FACTOR_LOWER_BND,_MATING_FACTOR_UPPER_BND);

        if (pre_age) {
            p->age = (unsigned short)GetRandomValue(_GEN_AGE_LOWER_BND, p->gen_age -1);
            if (p->age >= _FERTILITY_START) p->fertile = true;
        }

    }
    return p;
};


void pawn_age(Pawn *p){
    if (p) {
        if (p->alive) {
            p->age++;
            if (p->fertile) {
                p->fertility_factor = (unsigned short)round( _FERTILITY_DECAY * p->fertility_factor -1 );
            }
            if (p->age >= _FERTILITY_START && !p->fertile) p->fertile = true;
            if (p->fertility_factor == 0 && p->fertile) p->fertile = false;
        }
    }
};


void pawn_print(Pawn *p) {
    printf("id:%u, age:%u, alive:%d, bday:%u, (%d,%d), fertile:%d, fert_fact:%u, mating_rad:%u, mating_fact:%u, gen_age:%u\n",
        p->id,
        p->age,
        p->alive,
        p->bday,
        p->x_pos,
        p->y_pos,
        p->fertile,
        p->fertility_factor,
        p->mating_radius,
        p->mating_factor,
        p->gen_age
    );
}

void pawn_free(Pawn *p) {
    free(p);
}
