
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <raylib.h>
#include "../include/world.h"
#include "../include/util.h"
#include "../include/pawnvec.h"


static const int _WIN_WIDTH_OFFSET = 10;                                    // pixel offset from window width boundary for pawn population
static const int _WIN_HEIGHT_OFFSET = 10;                                   // pixel offset from window height boundary for pawn population
static const int _PAWN_SEARCH_RADIUS = 5;                                  // pixel radius to search around mid-point of parents
static const int _PAWN_MAX_POSSIBLE_MATES = 3;                              // the maximum number of possible mates a pawn can store in it's radius
static const int _PAWN_STARVE_PROBS[] = {0, 0, 1, 2, 3, 5, 10, 15};          // prob of pawn starving based on immediate ring population
static const int _PAWN_ATTACKED_PROBS[] = {0, 0, 0, 0, 1, 2, 3, 5};         // prob of pawn being attacked based on immediate ring population
static const int _PAWN_MIGRATION_RADIUS = 20;                               // max radius a pawn can migrate
static const int _PAWN_MIGRATION_PROB = 10;                                 // chance pawn migrates in a season
static const int _PAWN_MIGRATION_PROB_DENOM = 1000;                         // chance pawn migrates in a season
static const int _PAWN_RING_RADIUS = 1;                                     // radius around pawn to check for other pawns (for starve, and attack rolls)


static Point2d world_get_new_pawn_xy(World*, Pawn*, Pawn*);                 // locate a suitable xy coord for a new pawn to generate
static Point2d world_find_midpoint(Point2d, Point2d);                       // calcuate the mid-point between two xy Points
static Point2d world_region_search(World*, Point2d, int);                   // find empty cell in radius of point
static unsigned int world_calc_distance(Point2d, Point2d);                  // calcuate the distance between two xy Points
static bool world_is_cell_free(World*, Point2d);                            // check the cell for availability
static void world_reset_mated_flag(PawnVec*);                               // resets the mated flag for all the pawns
static int* world_random_list(int, int, int, int, int);                     // make an array of indices for populating world


World *world_new(int *err, int xw, int yh) {
    // check offsets fit in window dims
    if (xw - 2 * _WIN_WIDTH_OFFSET < 1 || yh - 2 * _WIN_HEIGHT_OFFSET < 1) {
        fprintf(stderr, "window of: %d x %d and offsets of %d, %d don't make sense\n", xw, yh, _WIN_WIDTH_OFFSET, _WIN_HEIGHT_OFFSET);
        exit(1);
    }

    World *new_world = malloc(sizeof(World));   // let the caller check for NULL
    if (new_world) {
        new_world->pawn_cnt = 0;
        new_world->alive_pawns = 0;
        new_world->born_pawns = 0;
        new_world->old_age_death = 0;
        new_world->starved_pawns = 0;
        new_world->attacked_pawns = 0;
        new_world->migrated_pawns = 0;
        new_world->season = 0;
        new_world->x_width = xw;
        new_world->y_height = yh;
        new_world->pawn_arr_len = new_world->x_width * new_world->y_height;

        // 2d pawn array (linear storage)
        new_world->pawns2d = calloc(new_world->pawn_arr_len, sizeof(Pawn*));
        if (!new_world->pawns2d) {
            *err = -1;
        }
    }

    return new_world;
}


void world_free(World *w) {
    for (int i = 0; i<w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) {
            free(w->pawns2d[i]);
        }
    }
    free(w->pawns2d);
    free(w);
}


void world_dump_data(World *w) {
    // when sim ends print out stats
    printf("\nLIFESIM STATS:\n------------------------------\n");
    printf("world dims: %d width x %d height\n",w->x_width, w->y_height);
    printf("seasons: %u\n", w->season);
    printf("all time pawns: %u\nliving pawns at end: %u\ndead pawns: %u\n", w->pawn_cnt, w->alive_pawns, w->pawn_cnt - w->alive_pawns);
    printf("pawns born: %u\n", w->born_pawns);
    printf("pawns migrated: %u\n", w->migrated_pawns);
    printf("pawns old age: %u\n", w->old_age_death);
    printf("pawns starved: %u\n", w->starved_pawns);
    printf("pawns killed: %u\n", w->attacked_pawns);
    printf("------------------------------\n");
}


void world_populate(World* w, int tot_pop) {
    int pawn_capacity = (w->x_width - 2 * _WIN_WIDTH_OFFSET) * (w->y_height - 2 * _WIN_HEIGHT_OFFSET);
    if (tot_pop > pawn_capacity) {
        fprintf(stderr, "%d is too many pawns for %d free space\n", tot_pop, pawn_capacity);
        exit(1);
    }
    
    int *rnd_arr = world_random_list(tot_pop, w->x_width, w->y_height, _WIN_WIDTH_OFFSET, _WIN_HEIGHT_OFFSET);
    int x_idx, y_idx;
    Pawn *tmp_pawn;

    for (int i = 0; i < w->pawn_arr_len; i++) {
        
        if (rnd_arr[i]) {

            x_idx = i % w->x_width;
            y_idx = i / w->x_width;

            // make new pawn
            tmp_pawn = pawn_new(w->pawn_cnt, x_idx, y_idx, 0, true);
            if (!tmp_pawn) {
                fprintf(stderr, "failed to allocate pawn %u at i-%d j-%d\n", w->pawn_cnt + 1, x_idx, y_idx);
                exit(1);
            }

            w->pawns2d[y_idx * w->x_width + x_idx] = tmp_pawn;
            (w->pawn_cnt)++;
            (w->alive_pawns)++;

        }
    }
    free(rnd_arr);
};


void world_update(World *w) {
    PawnVec *mate_list = pawnvec_new();
    PawnVec *born_list = pawnvec_new();
    PawnVec *dead_list = pawnvec_new();
    if (!mate_list || !born_list || !dead_list) {
        fprintf(stderr, "could not make PawnVec\n");
        exit(1);
    }
    MigVec *migrate_list = pawnvec_new_mg();
    if (!migrate_list) {
        fprintf(stderr, "could not make MigVec\n");
        exit(1);
    }

    // main update loop
    Pawn *p = NULL;
    for (int i = 0; i < w->pawn_arr_len; i++) {
        
        if(!w->pawns2d[i]) continue;                        // skip empty cells
        p = w->pawns2d[i];

        world_kill_pawn(w, w->pawns2d[i], dead_list);       // retire dead pawn **and kill pawns with too many neighbors**
        if (!p->alive) continue;                            // pawn is now dead

        world_mate(w, p, mate_list, born_list);             // mate a Pawn
        world_age_pawn(w, p);                               // age the pawns (who were not just born)
        world_find_migrating_pawns(w, p, migrate_list);     // add pawn to migration list if passes roll
        
    }

    // place the newborns
    if (born_list->len > 0) {
        for (int i = 0; i<born_list->len; i++) {
            world_add_pawn(w, born_list->ps[i]);
            w->born_pawns++;
        }
    }

    world_reset_mated_flag(mate_list);                      // use mate_list to reset the mated flag
    world_migrate_pawns(w, migrate_list);                   // move the pawns that are to migrate
    world_remove_dead_pawns(w, dead_list);                  // remove the dead pawns

    pawnvec_free(mate_list);
    pawnvec_free(born_list);
    pawnvec_free(dead_list);
    pawnvec_free_mg(migrate_list);

}


void world_add_new_pawn_by_point(World* w, Point2d p) {
    Pawn *tmp = pawn_new(w->pawn_cnt + 1, p.x, p.y, w->season, false);
    if(!tmp) {
        fprintf(stderr, "failed to allocate Pawn %u, at: (%d, %d)\n", w->pawn_cnt+1, p.x, p.y);
        exit(1);
    }
    w->pawns2d[convert_2d_to_1d_idx(p.x, p.y, w->x_width)] = tmp;
    w->pawn_cnt++;
    w->alive_pawns++;

}


void world_new_pawn(World* w, Point2d p, PawnVec *bornVec) {
    Pawn *tmp = pawn_new(w->pawn_cnt + 1, p.x, p.y, w->season, false);
    if(!tmp) {
        fprintf(stderr, "failed to allocate Pawn %u, at: (%d, %d)\n", w->pawn_cnt+1, p.x, p.y);
        exit(1);
    }
    pawnvec_add(bornVec, tmp);
}


void world_add_pawn(World* w, Pawn *p) {
    if (!p) return;
    w->pawns2d[convert_2d_to_1d_idx(p->x_pos, p->y_pos, w->x_width)] = p;
    w->pawn_cnt++;
    w->alive_pawns++;

}

void world_remove_dead_pawns(World *w, PawnVec *pv) {
    if (!pv) return;
    if (pv->len == 0) return;
    int idx;  

    Pawn *p = NULL;
    for (int i = 0; i<pv->len; i++) {
        p = pv->ps[i];
        idx = convert_2d_to_1d_idx(p->x_pos, p->y_pos, w->x_width);
        w->pawns2d[idx] = NULL;
        w->alive_pawns--;
        pawn_free(p);
    }
}


void world_age_pawn(World *w, Pawn* p) {
    if (p) {
        if (p->alive && p->bday < w->season) {
            pawn_age(p);
        }
    }
}


void world_find_migrating_pawns(World *w, Pawn *p, MigVec *mg) {
    
    if (!mg) {
        fprintf(stderr, "null mig vector!\n");
        return;
    }
    if (!p) {
        fprintf(stderr, "null pawn!\n");
        return;
    }

    if (p->alive) {

        // roll the migration check
        int rnd = GetRandomValue(1, _PAWN_MIGRATION_PROB_DENOM);
        if (rnd <= _PAWN_MIGRATION_PROB) {

            Vec2d v = generate_random_vector(_PAWN_MIGRATION_RADIUS);
            if ( v.x == 0 && v.y == 0) return;    // same cell
            if (v.x + p->x_pos < 0 || v.x + p->x_pos > w->x_width-1) return;      // off x boundary
            if (v.y + p->y_pos < 0 || v.y + p->y_pos > w->y_height-1) return;     // off y boundary

            Point2d pnt = {.x = p->x_pos + v.x, .y = p->y_pos + v.y};
            if( world_is_cell_free(w, pnt) ) {   // migration cell is free
                pawnvec_add_mg(mg, pawnvec_new_migpawn(p, pnt));
            }

        }

    }
}


void world_migrate_pawns(World *w, MigVec *mg) {
    if (mg->len == 0) return;

    int mig_idx, old_idx;
    for (int i = 0; i < mg->len; i++) {
        // place pawn if the location is free
        if(!world_is_cell_free(w, mg->ps[i]->mig_pnt)) continue;    // if the cell is occupied now; first come first served

        mig_idx = convert_2d_to_1d_idx(mg->ps[i]->mig_pnt.x, mg->ps[i]->mig_pnt.y, w->x_width);
        old_idx = convert_2d_to_1d_idx(mg->ps[i]->p->x_pos, mg->ps[i]->p->y_pos, w->x_width);

        // move pawn
        w->pawns2d[mig_idx] = mg->ps[i]->p;
        w->pawns2d[old_idx] = NULL;

        // update pawn coords
        mg->ps[i]->p->x_pos = mg->ps[i]->mig_pnt.x;
        mg->ps[i]->p->y_pos = mg->ps[i]->mig_pnt.y;

        w->migrated_pawns++;
    }
}


void world_kill_pawn(World *w, Pawn *p, PawnVec *dv) {

    if (!p || !dv) return;
    int rnd, cnt;
    //int idx = convert_2d_to_1d_idx(p->x_pos, p->y_pos, w->x_width);

    if(p->alive) {
        cnt = world_count_pawns_in_ring(w, p, _PAWN_RING_RADIUS);
        if (cnt > (_PAWN_RING_RADIUS * 2 + 1) * (_PAWN_RING_RADIUS * 2 + 1) -1) {  // avoid seg
            fprintf(stderr, "ring has more than (2r+1)^2-1 cells: %d returned\n", cnt);
            exit(1);
        }
        
        // dies of old age
        if (p->age >= p->gen_age) {
            p->alive = false;
            // w->alive_pawns--;
            w->old_age_death++;
            // w->pawns2d[idx] = NULL;
            // pawn_free(p);
            pawnvec_add(dv, p);
            return;
        }

        if (cnt == 0) return; // nothing to check;

        // dies of starvation
        rnd = GetRandomValue(1, 100);
        if (_PAWN_STARVE_PROBS[cnt - 1] >= rnd) {
            p->alive = false;
            // w->alive_pawns--;
            w->starved_pawns++;
            // w->pawns2d[idx] = NULL;
            // pawn_free(p);
            pawnvec_add(dv, p);
            return;
        }

        // dies of attack
        rnd = GetRandomValue(1, 100);
        if (_PAWN_ATTACKED_PROBS[cnt - 1] >= rnd) {
            p->alive = false;
            // w->alive_pawns--;
            w->attacked_pawns++;
            // w->pawns2d[idx] = NULL;
            // pawn_free(p);
            pawnvec_add(dv, p);
            return;
        }

    } else {
        fprintf(stderr, "pawn: %u at (%d, %d) still around but dead\n", p->id, p->x_pos, p->y_pos);
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
    Vec2d *pnts = generate_random_offsets(rad);

    for (int i = 0; i < pnt_cnt; i++) {
        if (x + pnts[i].x < 0 || x + pnts[i].x > w->x_width-1) continue;      // off x boundary
        if (y + pnts[i].y < 0 || y + pnts[i].y > w->y_height-1) continue;     // off y boundary

        idx = convert_2d_to_1d_idx(x + pnts[i].x, y + pnts[i].y, w->x_width);

        if (w->pawns2d[idx]) {
            if ( world_mate_check(p, w->pawns2d[idx]) ) {
                free(pnts);
                return w->pawns2d[idx];
            }
            mating_tries--;
            if (mating_tries == 0) {
                free(pnts);
                return ret_pawn;
            }
        }
    }

    free(pnts);

    return ret_pawn;
}

// add pawn collect, and pawn populate functions
void world_mate(World *w, Pawn *p, PawnVec *mate_vec, PawnVec *born_vec) {
    Pawn *mate;
    Point2d new_pawn_pt;
    
    if (!p || !mate_vec) return;    // is the cell or vector empty

    if (p->alive && p->fertile && !p->mated) {    // qualify pawn as matable

        // try to mate pawn
        mate = world_get_mate(w, p);
        if (!mate) return;    // no mate found

        // pawns mated; New Pawn, if there is space!
        new_pawn_pt = world_get_new_pawn_xy(w, p, mate);
        p->mated = true;
        mate->mated = true;
        pawnvec_add(mate_vec, p);
        pawnvec_add(mate_vec, mate);

        if (new_pawn_pt.x == -1 && new_pawn_pt.y == -1) {
            return;   // didn't find space to exist
        }

        // make new pawn
        world_new_pawn(w, new_pawn_pt, born_vec);

    }

}


int world_count_pawns_in_ring(World *w, Pawn *p, int r) {
    // ring is the (2r+1)^2-1 cells around the pawn
    if (!p) return 0;
    int px = p->x_pos, py = p->y_pos;
    int ring_cnt = 0;
    int idx;

    for (int x = -r; x <= r; x++) {
        for (int y = -r; y <= r; y++) {
            if (x == 0 && y == 0) continue; // skip ref cell
            if (x + px < 0 || x + px > w->x_width-1) continue;      // off x boundary
            if (y + py < 0 || y + py > w->y_height-1) continue;     // off y boundary

            idx = convert_2d_to_1d_idx(px+x, py+y, w->x_width);
            if (w->pawns2d[idx]) ring_cnt++;
        }
    }

    return ring_cnt;
}


bool world_audit_world(World *w) {
    unsigned int alive = 0, dead = 0;
    // check world stats for correctness
    for (int i = 0; i<w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) {
            if (w->pawns2d[i]->alive) alive++;
            else dead++;
        }
    }
    return w->alive_pawns == alive && dead == 0;
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
    Vec2d *pnts = generate_random_offsets(r);

    // find nearby point in radius
    for (int i = 0; i < pnt_cnt; i++) {

        tmp.x = p.x + pnts[i].x;
        tmp.y = p.y + pnts[i].y;
        if (tmp.x < 0 || tmp.y < 0) continue;   // if search cell extends beyond the window in either dim, ignore and continue
        if (tmp.x > w->x_width-1 || tmp.y > w->y_height-1) continue;

        if (world_is_cell_free(w, tmp)) {
            free(pnts);
            return tmp;
        }
    }
    free(pnts);
        
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
        fprintf(stderr, "ERR sqrt(neg): d[ (%d,%d), (%d,%d) ] -> %f\n", p1.x,p1.y,p2.x,p2.y, tmp);
        exit(1);
    }

    if (tmp > 0 && tmp < 1) return 1;    // avoid truncating to zero
  
    return (unsigned int)sqrtf(tmp);

}


static bool world_is_cell_free(World* w, Point2d p) {
    return w->pawns2d[p.y * w->x_width + p.x] == NULL;

}


static void world_reset_mated_flag(PawnVec *pv) {
    if (!pv) return;
    if (pv->len == 0) return;

    for (int i = 0; i < pv->len; i++) {
        
        if(pv->ps[i]->mated == true) {
            pv->ps[i]->mated = false;
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