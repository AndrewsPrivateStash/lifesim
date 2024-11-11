/*
    defines the world which is the collection of persons and any other environment details needed

*/


#ifndef MODEL_H_
#define MODEL_H_

#include "pawn.h"
#include "util.h"
#include "pawnvec.h"
#include <stdbool.h>


typedef struct World{
    unsigned int pawn_cnt;      // total count of pawns who have existed
    unsigned int alive_pawns;   // count of alive pawns
    unsigned int born_pawns;    // count of born pawns
    unsigned int old_age_death;     // count of pawns who die naturally
    unsigned int starved_pawns;     // count of starved pawns
    unsigned int attacked_pawns;    // count of attacked pawns
    unsigned int migrated_pawns;    // count of migrated pawns
    unsigned int season;        // every frame is a season
    int x_width;                // screen width
    int y_height;               // screen height
    int pawn_arr_len;           // length of pawns2d (x_width * y_height)
    Pawn **pawns2d;             // array of pointers to pawns stored linearly; use arr[row * xw + col] to index (row, col)

} World;


// ########## WORLD ##########
// makes new World object. err -1 indicates the Pawn array failed to allocate
World *world_new(int*, int, int);
// free the world object and associated pawn array
void world_free(World*);
// populate the world object with pawns randomly distributing them accross the passed window dimensions
void world_populate(World*, int tot_pop);
// dump world data to terminal when sim ends
void world_dump_data(World*);


// ########## MATING ##########
// mate the pawns looking in a grid around each based on the pawns mating radius
void world_mate(World*);
// look for mate in the grid around the pawn using radius
Pawn *world_get_mate(World*, Pawn*);
// roll the mating factor check
bool world_mating_factor_check(Pawn*, Pawn*);
// roll the fertility check
bool world_fertility_check(Pawn*, Pawn*);
// decide if the two pawns will mate
bool world_mate_check(Pawn*, Pawn*);


// ########## GENERAL ##########
// add a pawn to the pawns array at a given point
void world_add_pawn(World*, Point2d);
// scan pawns and retire those of old age
void world_kill_pawns(World*);
// age the alive pawns
void world_age_pawns(World*);
// random chance to migrate the pawn, returns array of pawns to migrate
PawnVec *world_find_migrating_pawns(World *w);
// migrate the found pawns
void world_migrate_pawns(World*);
// count the pawns in the ring around current pawn
int world_count_pawns_in_ring(World*, Pawn*, int);
// audit the world stats
bool world_audit_world(World*);

#endif