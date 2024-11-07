/*
    defines the world which is the collection of persons and any other environment details needed

*/


#ifndef MODEL_H_
#define MODEL_H_

#include "pawn.h"
#include <stdbool.h>


typedef struct World{
    unsigned int pawn_cnt;      // total count of pawns who have existed
    unsigned int alive_pawns;   // count of alive pawns
    unsigned int season;        // every tic is a season
    unsigned int capacity;      // capacity of Pawn array
    Pawn **pawns;               // array of pointers to pawns

} World;

// store an xy coordinate
typedef struct Point2d {
    int x;
    int y;
} Point2d;

// ########## WORLD ##########
// makes new World object. err -1 indicates the Pawn array failed to allocate
World *world_new(int*);
// populate the world object with pawns randomly distributing them accross the passed window dimensions
void world_populate(World*, int xmax, int ymax, unsigned int tot_pop, int pop_prob);
// reallocs the pawns array doubling the capacity (like std::vector or slice in Go)
Pawn **world_resize_pawns(World*);
// mate the Pawns in the world (exhaustive, checking all pawns O(n^2))


// ########## MATING ##########
void world_mate_ex(World*);
// mate the Pawns in the world (use likned list of mates held by pawn, checking up to 20)
void world_mate(World*);
// roll the mating factor check
bool world_mating_factor_check(Pawn*, Pawn*);
// roll the fertility check
bool world_fertility_check(Pawn*, Pawn*);
// decide if the two pawns will mate
bool world_mate_check(Pawn*, Pawn*);
// free the world object and associated pawn array

// ########## UTILS ##########
void world_free(World*);
// add a pawn to the pawns array at a given point
void world_add_pawn(World*, Point2d);
// scan pawns and retire those of old age
void world_kill_pawns(World*);
// age the alive pawns
void world_age_pawns(World*);
// populate mates list for a given pawn

// ########## POSSIBLE MATES LIST ##########
void world_get_mates(World*, Pawn*);
// update all pawn mating lists
void world_get_all_mates(World*);
// purge the dead from mating lists
void world_purge_mates(World*);


#endif