
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/world.h"
#include "../include/plist.h"
#include "../include/util.h"
#include "raylib.h"


const unsigned int _CAPACITY = 100;         // default starting capacity for pawn array
const int _WIN_WIDTH_OFFSET = 5;           // pixel offset from window width boundary for pawn population
const int _WIN_HEIGHT_OFFSET = 5;          // pixel offset from window height boundary for pawn population
const int _REALLOC_SCALE_FACTOR = 2;        // factor scaling the resizing of an array
const int _PAWN_SEARCH_RADIUS = 5;          // pixel radius to search around mid-point of parents
const int _PAWN_MAX_POSSIBLE_MATES = 5;     // the maximum number of possible mates a pawn can store in it's radius


static Point2d world_get_new_pawn_xy(World*, Pawn*, Pawn*);             // locate a suitable xy coord for a new pawn to generate
static Point2d world_find_midpoint(Point2d, Point2d);                   // calcuate the mid-point between two xy Points
static Point2d world_region_search(World*, Point2d, int);               // find empty cell in radius of point
static unsigned int world_calc_distance(Point2d, Point2d);              // calcuate the distance between two xy Points
static bool world_is_cell_free(World*, Point2d);                        // check the cell for availability
static void world_reset_mated_flag(World*);                             // resets the mated flag for all the pawns
static int* world_random_list(int, int, int, int, int);                 // make an array of indices for populating world


// allocate a new world
World *world_new(int *err) {
    World *new_world = malloc(sizeof(World));   // let the caller check for NULL
    if (new_world) {
        new_world->pawn_cnt = 0;
        new_world->alive_pawns = 0;
        new_world->season = 0;
        new_world->capacity = _CAPACITY;

        new_world->pawns = malloc(sizeof(Pawn*) * new_world->capacity);  //starting capacity, resized if needed
        if (!new_world->pawns) {
            *err = -1;
        }

    }

    return new_world;
}


void world_free(World *w) {
    for (int i = 0; i < w->pawn_cnt; i++) {
        pawn_free(w->pawns[i]);
    }
    free(w);
}


Pawn** world_resize_pawns(World *w) {
    Pawn **tmp = realloc(w->pawns, sizeof(Pawn*) * w->capacity * _REALLOC_SCALE_FACTOR);
    if (!tmp) {
        fprintf(stderr, "failed to reallocate pawns array in world object\n");
        exit(1);
    }
    w->capacity *= _REALLOC_SCALE_FACTOR;
    return tmp;
}


void world_populate(World* w, int xmax, int ymax, int tot_pop) {

    if (xmax - 2 * _WIN_WIDTH_OFFSET < 1 || ymax - 2 * _WIN_HEIGHT_OFFSET < 1) {
        fprintf(stderr, "window of: %d x %d and offsets of %d, %d don't make sense\n", xmax, ymax, _WIN_WIDTH_OFFSET, _WIN_HEIGHT_OFFSET);
        exit(1);
    }
    
    int *rnd_arr = world_random_list(tot_pop, xmax, ymax, _WIN_WIDTH_OFFSET, _WIN_HEIGHT_OFFSET);
    if (w->capacity == w->pawn_cnt) w->pawns = world_resize_pawns(w);
    
    int x_idx, y_idx;
    Pawn *tmp_pawn;

    for (int i = 0; i < xmax * ymax; i++) {
        
        if (rnd_arr[i]) {

            x_idx = i % xmax;
            y_idx = i / xmax;

            // makw new pawn
            tmp_pawn = pawn_new(w->pawn_cnt, x_idx, y_idx, 0, true);
            if (!tmp_pawn) {
                fprintf(stderr, "failed to allocate pawn %u at i-%d j-%d\n", w->pawn_cnt + 1, x_idx, y_idx);
                continue;
            }

            w->pawns[w->pawn_cnt] = tmp_pawn;
            (w->pawn_cnt)++;
            (w->alive_pawns)++;
            if (w->capacity == w->pawn_cnt) w->pawns = world_resize_pawns(w);

        }
    }
    free(rnd_arr);
};


void world_add_pawn(World* w, Point2d p) {
    if (w->capacity == w->pawn_cnt) w->pawns = world_resize_pawns(w);   // check array capacity first
    Pawn *tmp = pawn_new(w->pawn_cnt + 1, p.x, p.y, w->season, false);
    if(!tmp) {
        fprintf(stderr, "failed to allocate Pawn %u, at: (%d, %d)\n", w->pawn_cnt+1, p.x, p.y);
        return;
    }
    w->pawns[w->pawn_cnt] = tmp;
    (w->pawn_cnt)++;
    (w->alive_pawns)++;

}


void world_age_pawns(World* w) {
    for (int i = 0; i < w->pawn_cnt; i++) {
        if (w->pawns[i]->alive && w->pawns[i]->bday < w->season) {
            pawn_age(w->pawns[i]);
        }
    }
}


void world_kill_pawns(World* w) {
    Pawn *p;
    for (int i = 0; i< w->pawn_cnt; i++) {
        if(w->pawns[i]->alive) {
            p = w->pawns[i];
            if (p->age >= p->gen_age) {
                p->alive = false;
                p->fertility_factor = 0;
                p->mating_factor = 0;
                p->mating_radius = 0;
                (w->alive_pawns)--;
            }
        }
    }
}


bool world_mating_factor_check(Pawn *p1, Pawn *p2) {
    int rnd;

    // roll the mating check for both pawns
    rnd = GetRandomValue(0, 100);
    if (p1->mating_factor >= rnd) {
        rnd = GetRandomValue(0, 100);
        if (p2->mating_factor >= rnd) {
            return true;
        }
    }
    return false;
}


bool world_fertility_check(Pawn *p1, Pawn *p2) {
    int rnd, avg;

    // average the fertility stats and roll
    avg = ( p1->fertility_factor + p2->fertility_factor ) / 2;
    rnd = GetRandomValue(0, 100);
    if (avg >= rnd)  return true;
    return false;
}


bool world_mate_check(Pawn *p1, Pawn *p2) {
    // assumes the two pawns are inside each others mating radius
    if ( world_mating_factor_check(p1, p2) ) {
        if ( world_fertility_check(p1, p2) ) {
            return true;
        }
    }
    return false;
}


void world_mate(World *w) {
    Pawn *p;
    Point2d new_pawn_pt;
    int bnd = w->pawn_cnt;
    Plist *pawn_mates = NULL;
    Pnode *cur = NULL;

    for (int i = 0; i < bnd - 1; i++) {     // loop over all of the existing pawns

        if (w->pawns[i]->alive && w->pawns[i]->fertile && !w->pawns[i]->mated) {    // qualify pawn as matable
            p = w->pawns[i];
            if (!p->possible_mates->head) continue;     // no mates

            pawn_mates = p->possible_mates;
            cur = pawn_mates->head;

            while (cur) {   // check each of the possible mates for the pawn
                if ( world_mate_check(p, cur->pwn) ) {
                    // pawns mated; New Pawn, if there is space!
                    new_pawn_pt = world_get_new_pawn_xy(w, p, cur->pwn);
                    p->mated = true;
                    cur->pwn->mated = true;

                    if (new_pawn_pt.x == -1 && new_pawn_pt.y == -1) {
                        break;   // didn't find space to exist
                    }

                    // make new pawn
                    world_add_pawn(w, new_pawn_pt);
                    break;

                }
                cur = cur->next;
            }
        }
    }
    world_reset_mated_flag(w);
}


void world_mate_ex(World *w) {

    Point2d p1, p2, new_pawn_pt;
    unsigned int dist, avg;
    int rnd;
    int bnd = w->pawn_cnt;

    for (int i = 0; i < bnd - 1; i++) {

        if (w->pawns[i]->alive && w->pawns[i]->fertile && !w->pawns[i]->mated) {
            p1.x = w->pawns[i]->x_pos;
            p1.y = w->pawns[i]->y_pos;

            for (int j = i+1; j < bnd; j++) {

                if (w->pawns[j]->alive && w->pawns[j]->fertile && !w->pawns[i]->mated) {
                    p2.x = w->pawns[j]->x_pos;
                    p2.y = w->pawns[j]->y_pos;

                    dist = world_calc_distance(p1, p2);     // distance between i and j pawn

                    // check mating radius for both pawns
                    if ( dist <= w->pawns[i]->mating_radius && dist <= w->pawns[j]->mating_radius) {
                        
                        // roll the mating check for both pawns
                        rnd = GetRandomValue(0, 100);
                        if (w->pawns[i]->mating_factor >= rnd) {
                            rnd = GetRandomValue(0, 100);
                            if (w->pawns[j]->mating_factor >= rnd) {
                                
                                // average the fertility stats and roll
                                avg = ( w->pawns[i]->fertility_factor + w->pawns[j]->fertility_factor ) / 2;
                                rnd = GetRandomValue(0, 100);
                                if (avg >= rnd) {
                                                                        
                                    // New Pawn, if there is space!
                                    new_pawn_pt = world_get_new_pawn_xy(w, w->pawns[i], w->pawns[j]);
                                    w->pawns[i]->mated = true;
                                    w->pawns[j]->mated = true;

                                    if (new_pawn_pt.x == -1 && new_pawn_pt.y == -1) {
                                        break;   // didn't find space to exist
                                    }

                                    // make new pawn
                                    world_add_pawn(w, new_pawn_pt);
                                    break;  // continue to i-loop
                                    
                                }
                            }
                        }
                    }       
                }
            }
        }
    }

    world_reset_mated_flag(w);
};

void world_get_mates(World *w, Pawn *p) {
    // find all pawns inside mating radius and add to mating list for single pawn

    if (p->possible_mates->cnt == _PAWN_MAX_POSSIBLE_MATES) return;     // pawn at max possible mates

    Point2d ref_point = {p->x_pos, p->y_pos};
    Point2d comp_point;
    Pawn *cp;
    unsigned short dist;

    for (int i = 0; i < w->pawn_cnt; i++) {
        if (w->pawns[i] == p) continue;
        cp = w->pawns[i];

        if (cp->alive && cp->fertile) {
            if (plist_inlist(p->possible_mates, cp)) continue;  // already in list

            comp_point.x = cp->x_pos;
            comp_point.y = cp->y_pos;
            dist = world_calc_distance(ref_point, comp_point);

            if ( dist <= p->mating_radius && dist <= cp->mating_radius ) {
                // comparison pawn is inside the radius 
                plist_add_node(p->possible_mates, cp, dist);
            }

        }
    }
}


void world_get_all_mates(World *w) {
    for (int i = 0; i < w->pawn_cnt; i++) {
        if (w->pawns[i]->alive && w->pawns[i]->fertile) {
            world_get_mates(w, w->pawns[i]);
        }
    }
}


void world_purge_mates(World *w) {
    for (int i = 0; i < w->pawn_cnt; i++) {
        if (w->pawns[i]->alive) {
            plist_purge(w->pawns[i]->possible_mates);   
        }
    }
}








// ########## location functions ##########

static Point2d world_get_new_pawn_xy(World* w, Pawn* pn1, Pawn* pn2) {
    // get mid-point of parents
    // find free cell in radius if exists
    Point2d p1 = {pn1->x_pos, pn1->y_pos};
    Point2d p2 = {pn2->x_pos, pn2->y_pos};
    Point2d mid_point = world_find_midpoint(p1, p2);
    return world_region_search(w, mid_point, _PAWN_SEARCH_RADIUS);

}


static Point2d world_region_search(World* w, Point2d p, int r) {
    if ( world_is_cell_free(w, p) ) {
        return p;
    }

    Point2d tmp;
    // find nearby point in radius
    for (int i = -r; i <= r; i++) {         //x (-r..r)
        for (int j = -r; j <= r; j++) {     //y (-r..r)
            if (i == 0 && j == 0) continue;
            tmp.x = p.x + i;
            tmp.y = p.y + j;
            if (tmp.x < 0 || tmp.y < 0) continue;   // if search cell extends beyond the window in either dim, ignore and continue

            if (world_is_cell_free(w, tmp)) {
                return tmp;
            }
        }
    }
    tmp.x = -1; tmp.y = -1;
    return tmp;     // failed to find cell, returns (-1,-1)

}


static Point2d world_find_midpoint(Point2d p1, Point2d p2) {
    // construct vector between the points and half to find nearest cell
    Vector2 disp_vec = {
            0.5 * p1.x + 0.5 * p2.x,
            0.5 * p1.y + 0.5 * p2.y
    };
    
    return (Point2d){ (int)disp_vec.x, (int)disp_vec.y };

}


static unsigned int world_calc_distance(Point2d p1, Point2d p2) {
    // (y2-y1)^2+(x2-x1)^2 = d^2
    if (p1.x == p2.x && p1.y == p2.y) return 0;

    float tmp = (float)((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
    if (tmp < 0) {
        fprintf(stderr, "ERR sqrt(neg): d[ (%d,%d), (%d,%d) ] -> %f", p1.x,p1.y,p2.x,p2.y, tmp);
        exit(1);
    }

    if (tmp >0 && tmp < 1) return 1;    // avoid truncating to zero
  
    return (unsigned short)sqrtf(tmp);

}


static bool world_is_cell_free(World* w, Point2d p) {
    for (int i = 0; i < w->pawn_cnt; i++) {
        if (w->pawns[i]->alive) {
            if ( p.x == w->pawns[i]->x_pos && p.y == w->pawns[i]->y_pos ) {
                return false;
            }
        }
    }
    return true;
}

static void world_reset_mated_flag(World* w) {
    for (int i = 0; i < w->pawn_cnt; i++) {
        if(w->pawns[i]->mated){
            w->pawns[i]->mated = false;
        }
    }
}


static int* world_random_list(int elems, int xw, int yh, int xbuff, int ybuff) {
    // elems count of initial pawns to distribute
    // xw screen width px
    // yh screen height px
    // xbuff left and right px buffer regions (symetric)
    // ybuff top and bottom px buffer regions (symetrci)
    
    int len = (xw - 2 * xbuff) * (yh - 2 * ybuff);          // starting array size
    int *arr = calloc(len, sizeof(int));                    // inner array
    int *arr_out = calloc(xw * yh, sizeof(int));            // outer array

    if (!arr || !arr_out) {
        fprintf(stderr, "failed to allocate population random array\n");
        exit(1);
    }

    for (int i = 0; i < elems; i++) { // populate array with 1s in the first elem indexes
        arr[i] = 1;
    }

    shuffle_ints(arr, len);     // shuffle array

    int idx_o, idx_a;
    // populate outer array
    for (int row = 0; row < yh; row++) {      
        for (int col = 0; col < xw; col++) {
            
            if (row < ybuff || row > yh - ybuff - 1) continue;   // top and buttom buffers
            if (col < xbuff || col > xw - xbuff - 1) continue;   // left and right buffers

            idx_o = row * xw + col;
            idx_a = (row - ybuff) * (xw - 2 * xbuff) + (col - xbuff);
            arr_out[idx_o] = arr[idx_a]; 
        }
    }

    free(arr);
    return arr_out;

}