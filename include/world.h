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

// makes new World object. err -1 indicates the Pawn array failed to allocate
World *world_new(int*);
// populate the world object with pawns randomly distributing them accross the passed window dimensions
void world_populate(World*, int xmax, int ymax, unsigned int tot_pop, int pop_prob);
// reallocs the pawns array doubling the capacity (like std::vector or slice in Go)
Pawn **world_resize_pawns(World*);
// mate the Pawns in the world
void world_mate(World*);
// free the world object and associated pawn array
void world_free(World*);
void world_add_pawn(World*, Point2d);                           // add a pawn to the pawns array at a given point
void world_kill_pawns(World*);                                  // scan pawns and retire those of old age
void world_age_pawns(World*);                                   // age the alive pawns


#endif