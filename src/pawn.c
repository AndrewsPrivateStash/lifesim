
#include "../include/pawn.h"
#include "../include/config.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

extern Config *CONFIG;


// ############### Constants ###############
// CONFIG->_GEN_AGE_LOWER_BND = 1;           // min genetic age of Pawn
// CONFIG->_GEN_AGE_UPPER_BND = 80;          // max genetic age of Pawn
// CONFIG->_FERT_FACT_LOWER_BND = 0;         // min fertility factor of Pawn
// CONFIG->_FERT_FACT_UPPER_BND = 100;       // max fertility factor of Pawn
// CONFIG->_MATING_RADIUS_LOWER_BND = 1;     // min mating radius of Pawn
// CONFIG->_MATING_RADIUS_UPPER_BND = 10;    // max mating radius of Pawn
// CONFIG->_MATING_FACTOR_LOWER_BND = 0;     // min mating factor of Pawn
// CONFIG->_MATING_FACTOR_UPPER_BND = 100;   // max mating factor of Pawn

// CONFIG->_FERTILITY_DECAY = 0.95;          // the decay factor applied to fertility factor each season
// CONFIG->_FERTILITY_START = 13;            // the age a Pawn becomes fertile


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
        p->gen_age = (unsigned short)GetRandomValue(CONFIG->_GEN_AGE_LOWER_BND,CONFIG->_GEN_AGE_UPPER_BND);
        p->fertility_factor = (unsigned short)GetRandomValue(CONFIG->_FERT_FACT_LOWER_BND,CONFIG->_FERT_FACT_UPPER_BND);    // decreases with age, dormant until CONFIG->_FERTILITY_START
        p->mating_radius = (unsigned short)GetRandomValue(CONFIG->_MATING_RADIUS_LOWER_BND,CONFIG->_MATING_RADIUS_UPPER_BND);
        p->mating_factor = (unsigned short)GetRandomValue(CONFIG->_MATING_FACTOR_LOWER_BND,CONFIG->_MATING_FACTOR_UPPER_BND);

        if (pre_age) {
            p->age = (unsigned short)GetRandomValue(CONFIG->_GEN_AGE_LOWER_BND, p->gen_age -1);
            if (p->age >= CONFIG->_FERTILITY_START) p->fertile = true;
        }

    }
    return p;
};


void pawn_free(Pawn *p) {
    free(p);
}


void pawn_age(Pawn *p){
    if (p) {
        if (p->alive) {
            p->age++;
            if (p->fertile) {
                p->fertility_factor = (unsigned short)round( CONFIG->_FERTILITY_DECAY * p->fertility_factor -1 );
            }
            if (p->age >= CONFIG->_FERTILITY_START && !p->fertile) p->fertile = true;
            if (p->fertility_factor == 0 && p->fertile) p->fertile = false;
        }
    }
};


void pawn_print(Pawn *p) {
    if (!p) return;
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


Pawn *pawn_copy(Pawn *p) {
    if (!p) return p;
    Pawn *copied_pawn = pawn_new(p->id, p->x_pos, p->y_pos, p->bday, false);
    copied_pawn->age = p->age;
    copied_pawn->alive = p->alive;
    copied_pawn->fertile = p->fertile;
    copied_pawn->fertility_factor = p->fertility_factor;
    copied_pawn->gen_age = p->gen_age;
    copied_pawn->mated = p->mated;
    copied_pawn->mating_factor = p->mating_factor;
    copied_pawn->mating_radius = p->mating_radius;
    
    return copied_pawn;
}