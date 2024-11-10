
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/world.h"
#include "../include/util.h"
#include "raylib.h"


const int _WIN_WIDTH_OFFSET = 5;                // pixel offset from window width boundary for pawn population
const int _WIN_HEIGHT_OFFSET = 5;               // pixel offset from window height boundary for pawn population
//const int _REALLOC_SCALE_FACTOR = 2;          // factor scaling the resizing of an array
const int _PAWN_SEARCH_RADIUS = 5;              // pixel radius to search around mid-point of parents
const int _PAWN_MAX_POSSIBLE_MATES = 5;         // the maximum number of possible mates a pawn can store in it's radius


static Point2d world_get_new_pawn_xy(World*, Pawn*, Pawn*);             // locate a suitable xy coord for a new pawn to generate
static Point2d world_find_midpoint(Point2d, Point2d);                   // calcuate the mid-point between two xy Points
static Point2d world_region_search(World*, Point2d, int);               // find empty cell in radius of point
static unsigned int world_calc_distance(Point2d, Point2d);              // calcuate the distance between two xy Points
static bool world_is_cell_free(World*, Point2d);                        // check the cell for availability
static void world_reset_mated_flag(World*);                             // resets the mated flag for all the pawns
static int* world_random_list(int, int, int, int, int);                 // make an array of indices for populating world


// allocate a new world
World *world_new(int *err, int xw, int yh) {
    World *new_world = malloc(sizeof(World));   // let the caller check for NULL
    if (new_world) {
        new_world->pawn_cnt = 0;
        new_world->alive_pawns = 0;
        new_world->born_pawns = 0;
        new_world->season = 0;
        new_world->x_width = xw;
        new_world->y_height = yh;
        new_world->pawn_arr_len = new_world->x_width * new_world->y_height;

        // 2d pawn array (linear storage)
        new_world->pawns2d = malloc(sizeof(Pawn*) * new_world->pawn_arr_len);
        if (!new_world->pawns2d) {
            *err = -1;
        }
        for (int i = 0; i < xw * yh; i++) {
            new_world->pawns2d[i] = NULL;
        }

    }

    return new_world;
}


void world_free(World *w) {
    for (int i = 0; i < w->pawn_arr_len; i++) {
        pawn_free(w->pawns2d[i]);
    }
    free(w);
}

void world_dump_data(World *w) {
    // when sim ends print out stats
    printf("\nLIFESIM STATS:\n------------------------------\n");
    printf("world dims: %d width x %d height\n",w->x_width, w->y_height);
    printf("seasons: %u\n", w->season);
    printf("all time pawns: %u\nliving pawns at end: %u\ndead pawns: %u\n", w->pawn_cnt, w->alive_pawns, w->pawn_cnt - w->alive_pawns);
    printf("pawns born: %u\n", w->born_pawns);
    printf("------------------------------\n");
}


// Pawn** world_resize_pawns(World *w) {
//     Pawn **tmp = realloc(w->pawns, sizeof(Pawn*) * w->capacity * _REALLOC_SCALE_FACTOR);
//     if (!tmp) {
//         fprintf(stderr, "failed to reallocate pawns array in world object\n");
//         exit(1);
//     }
//     w->capacity *= _REALLOC_SCALE_FACTOR;
//     return tmp;
// }


void world_populate(World* w, int tot_pop) {

    // check offsets fit in window dims
    if (w->x_width - 2 * _WIN_WIDTH_OFFSET < 1 || w->y_height - 2 * _WIN_HEIGHT_OFFSET < 1) {
        fprintf(stderr, "window of: %d x %d and offsets of %d, %d don't make sense\n", w->x_width, w->y_height, _WIN_WIDTH_OFFSET, _WIN_HEIGHT_OFFSET);
        exit(1);
    }
    
    int *rnd_arr = world_random_list(tot_pop, w->x_width, w->y_height, _WIN_WIDTH_OFFSET, _WIN_HEIGHT_OFFSET);
    
    int x_idx, y_idx;
    Pawn *tmp_pawn;

    for (int i = 0; i < w->pawn_arr_len; i++) {
        
        if (rnd_arr[i]) {

            x_idx = i % w->x_width;
            y_idx = i / w->x_width;

            // makw new pawn
            tmp_pawn = pawn_new(w->pawn_cnt, x_idx, y_idx, 0, true);
            if (!tmp_pawn) {
                fprintf(stderr, "failed to allocate pawn %u at i-%d j-%d\n", w->pawn_cnt + 1, x_idx, y_idx);
                continue;
            }

            w->pawns2d[y_idx * w->x_width + x_idx] = tmp_pawn;
            (w->pawn_cnt)++;
            (w->alive_pawns)++;

        }
    }
    free(rnd_arr);
};


void world_add_pawn(World* w, Point2d p) {
    Pawn *tmp = pawn_new(w->pawn_cnt + 1, p.x, p.y, w->season, false);
    if(!tmp) {
        fprintf(stderr, "failed to allocate Pawn %u, at: (%d, %d)\n", w->pawn_cnt+1, p.x, p.y);
        return;
    }
    w->pawns2d[p.y * w->x_width + p.x] = tmp;
    (w->pawn_cnt)++;
    (w->alive_pawns)++;

}


void world_age_pawns(World* w) {
    for (int i = 0; i < w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) {
            if (w->pawns2d[i]->alive && w->pawns2d[i]->bday < w->season) {
                pawn_age(w->pawns2d[i]);
            }
        }
    }
}


void world_kill_pawns(World* w) {
    Pawn *p;
    for (int i = 0; i< w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) {    //is cell occupied

            if(w->pawns2d[i]->alive) {
                p = w->pawns2d[i];
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
    
    unsigned int dist = world_calc_distance((Point2d){p1->x_pos, p1->y_pos}, (Point2d){p2->x_pos, p2->y_pos});
    if ( dist > p1->mating_radius || dist > p2->mating_radius ) return false;

    // assumes the two pawns are inside each others mating radius
    if ( world_mating_factor_check(p1, p2) ) {
        if ( world_fertility_check(p1, p2) ) {
            return true;
        }
    }
    return false;
}

Pawn *world_get_mate(World *w, Pawn *p) {
    // check mates in radius and return a pawn if found
    int rad = p->mating_radius;
    int x = p->x_pos, y = p->y_pos;
    int idx;
    int mating_tries = _PAWN_MAX_POSSIBLE_MATES;
    Pawn *ret_pawn = NULL;
    

    // randomize check order otherwise bias migrate NW
    int pnt_cnt = (rad*2+1) * (rad*2+1) -1;
    Point2d *pnts = generate_random_offsets(rad);

    for (int i = 0; i < pnt_cnt; i++) {
        if (x + pnts[i].x < 0 || x + pnts[i].x > w->x_width-1) continue;      // off x boundary
        if (y + pnts[i].y < 0 || y + pnts[i].y > w->y_height-1) continue;     // off y boundary

        idx = (y + pnts[i].y) * w->x_width + (x + pnts[i].x);

        if (w->pawns2d[idx]) {
            if ( world_mate_check(p, w->pawns2d[idx]) ) return w->pawns2d[idx];
            mating_tries--;
            if (mating_tries == 0) return ret_pawn;
        }
    }

    free(pnts);

    return ret_pawn;
}

void world_mate(World *w) {
    Pawn *p, *mate;
    Point2d new_pawn_pt;

    // loop over all of the window cells
    for (int i = 0; i < w->pawn_arr_len; i++) {
        if (w->pawns2d[i]){ // is the cell populated
            if (w->pawns2d[i]->alive && w->pawns2d[i]->fertile && !w->pawns2d[i]->mated) {    // qualify pawn as matable
                p = w->pawns2d[i];

                // try to mate pawn
                mate = world_get_mate(w, p);
                if (!mate) continue;    // no mate found

                // pawns mated; New Pawn, if there is space!
                new_pawn_pt = world_get_new_pawn_xy(w, p, mate);
                p->mated = true;
                mate->mated = true;

                if (new_pawn_pt.x == -1 && new_pawn_pt.y == -1) {
                    continue;   // didn't find space to exist
                }

                // make new pawn
                world_add_pawn(w, new_pawn_pt);
                w->born_pawns++;

            }       

        }
    }
    world_reset_mated_flag(w);

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
    int pnt_cnt = (2 * r + 1) * (2 * r + 1) -1;
    Point2d *pnts = generate_random_offsets(r);

    // find nearby point in radius
    for (int i = 0; i < pnt_cnt; i++) {

        tmp.x = p.x + pnts[i].x;
        tmp.y = p.y + pnts[i].y;
        if (tmp.x < 0 || tmp.y < 0) continue;   // if search cell extends beyond the window in either dim, ignore and continue
        if (tmp.x > w->x_width-1 || tmp.y > w->y_height-1) continue;

        if (world_is_cell_free(w, tmp)) {
            return tmp;
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
    return w->pawns2d[p.y * w->x_width + p.x] == NULL;

}

static void world_reset_mated_flag(World* w) {
    for (int i = 0; i < w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) {
            if(w->pawns2d[i]->mated) {
                w->pawns2d[i]->mated = false;
            }
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