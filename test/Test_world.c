/*
    test the various world functions
*/

#include <unity/unity.h>
#include <raylib.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../include/world.h"


void setUp(void) {
    /* This is run before EACH TEST */
}

void tearDown(void) {}



static int count_nonnulls_in_world(World *w) {
    int cnt = 0;
    for (int i = 0; i<w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) cnt++;
    }
    return cnt;
}

void test_function_world_populate(void) {
    int x = 50, y = 50, pawn_cnt = 10;
    int e = 0;
    World *w = world_new(&e, x, y);
    TEST_ASSERT_EQUAL_INT32(0, e);

    int not_null_cnt = 0;
    for (int i = 0; i<w->pawn_arr_len; i++) {
        if (w->pawns2d[i] != NULL) not_null_cnt++;
    }
    TEST_ASSERT_EQUAL_INT32(0, not_null_cnt);

    world_populate(w, 10);
    TEST_ASSERT_EQUAL_INT32(pawn_cnt, w->pawn_cnt);
    TEST_ASSERT_EQUAL_INT32(pawn_cnt, w->alive_pawns);
    TEST_ASSERT_EQUAL_INT32(x * y, w->pawn_arr_len);
    TEST_ASSERT_TRUE(w->pawns2d != NULL);
    TEST_ASSERT_EQUAL_INT32(pawn_cnt, count_nonnulls_in_world(w));
    TEST_ASSERT_EQUAL_INT32(0, w->season);
    TEST_ASSERT_EQUAL_INT32(x, w->x_width);
    TEST_ASSERT_EQUAL_INT32(y, w->y_height);
    TEST_ASSERT_EQUAL_INT32(0, w->attacked_pawns);
    TEST_ASSERT_EQUAL_INT32(0, w->born_pawns);
    TEST_ASSERT_EQUAL_INT32(0, w->migrated_pawns);
    TEST_ASSERT_EQUAL_INT32(0, w->old_age_death);
    TEST_ASSERT_EQUAL_INT32(0, w->starved_pawns);

    world_free(w);

}


static World *new_populated_test_world(int x, int y, int pawn_cnt) {
    int e = 0;
    World *w = world_new(&e, x, y);
    if(e) exit(1);
    world_populate(w, pawn_cnt);
    return w;
}

static void load_addresses(World *w, void **ab) {
    ab[0] = (void*) w;
    for (int i = 0; i<w->pawn_arr_len; i++) {
        ab[i+1] = (void*) w->pawns2d[i];
    }
}

static int compare_addresses(void **ab1, void **ab2, int len) {
    int match_cnt = 0;
    for (int i = 0; i<len; i++) {
        if (ab1[1] || ab2[i]) {
            printf("ad1_%d: %p\tad2_%d: %p\n", i, ab1[i], i, ab2[i]);
        }
        if (ab1[i] == ab2[i]) match_cnt++;
    }
    return match_cnt;
}

static int find_empty_cell_idx(World *w) {
    int free_idx = -1;
    for (int i = 0; i<w->pawn_arr_len; i++) {
        if (w->pawns2d[i] == NULL) {
            return i;
        }
    }
    return free_idx;
}
    
void test_function_world_add_new_pawn_by_point(void) {
    
    World *w = new_populated_test_world(30,30,10);
    int pawn_cnt = w->pawn_cnt;
    
    // room for pawn array plus world pointer
    void **add_buff_start = calloc(w->pawn_arr_len + 1, sizeof(void*));
    void **add_buff_end = calloc(w->pawn_arr_len + 1, sizeof(void*));
    load_addresses(w, add_buff_start);

    // find empty cell
    int free_idx = find_empty_cell_idx(w);
    
    Point2d new_pt = convert_1d_to_2d_idx(free_idx, w->x_width);
    printf("New Point: idx; %d\t Point: (%d,%d)\n", free_idx, new_pt.x, new_pt.y);
    world_add_new_pawn_by_point(w, new_pt);
    load_addresses(w, add_buff_end);
    
    // check prior memory state against new state
    int add_match_cnt = compare_addresses(add_buff_start, add_buff_end, w->pawn_arr_len+1);
    TEST_ASSERT_EQUAL_INT32(w->pawn_arr_len, add_match_cnt);

    // check pawn_cnt and alive_pawns cnt
    TEST_ASSERT_EQUAL_INT32(pawn_cnt+1, w->pawn_cnt);
    TEST_ASSERT_EQUAL_INT32(pawn_cnt+1, w->alive_pawns);


    world_free(w);
    free(add_buff_start);
    free(add_buff_end);
    
}


static void grab_ages(World *w, int *ar) {
    int idx = 0;
    for (int i = 0; i < w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) {
            ar[idx] = w->pawns2d[i]->age;
            printf("age_%d: %d\tstored:%d\n", idx, w->pawns2d[i]->age, ar[idx]);
            idx++;
        }
    }
}

static int compare_ages(int *ar1, int *ar2, int len, int offset) {
    int cnt = 0;
    for (int i = 0; i<len; i++) {
        if (ar1[i] + offset == ar2[i]) cnt++;
    }
    return cnt;
}

void test_function_world_age_pawn(void) {

    World *w = new_populated_test_world(30,30,10);
    int *ages_start = calloc(w->pawn_cnt, sizeof(int));
    int *ages_end = calloc(w->pawn_cnt, sizeof(int));

    grab_ages(w, ages_start);
    w->season++;
    for (int i = 0; i<w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) world_age_pawn(w, w->pawns2d[i]);
    }
    grab_ages(w, ages_end);

    int compare_cnt = compare_ages(ages_start, ages_end, w->pawn_cnt, 1);
    TEST_ASSERT_EQUAL_INT32(w->pawn_cnt, compare_cnt);

    world_free(w);
    free(ages_start);
    free(ages_end);
}


void test_function_world_find_migrating_pawns(void) {
    World *w = new_populated_test_world(40,40,20);
    MigVec *vec = pawnvec_new_mg();

    // roll until all pawns will migrate, check migvec
    for (int i = 0; i<w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) {
            int cur_len = vec->len;
            while (vec->len == cur_len) {
                world_find_migrating_pawns(w,w->pawns2d[i],vec);
            }
        }
    }
    pawnvec_print_mg(vec);
    TEST_ASSERT_EQUAL_INT32(w->pawn_cnt, vec->len);

    world_free(w);
    pawnvec_free_mg(vec);
}



static void load_addresses_pawns(World *w, void **ab) {
    for (int i = 0; i<w->pawn_arr_len; i++) {
        ab[i] = (void*) w->pawns2d[i];
    }
}

static int check_pawn_addresses(void **ab1, void **ab2, int len) {
 int match_cnt = 0;
 bool fnd = false;
    for (int i = 0; i<len; i++) {
        if (ab1[i] == NULL) continue;
        fnd = false;
        printf("looking for: %p at:%d: ", ab1[i], i);
        for (int j = 0; j<len; j++) {
            if (ab1[i] == ab2[j]) {
                printf("found at:%d\n", j);
                match_cnt++;
                fnd = true;
                break;
            }
        }
        if (!fnd) {
            printf("not found!\n");
        }
    }
    return match_cnt;   
}

void test_function_world_migrate_pawns(void) {
    World *w = new_populated_test_world(40,40,20);
    MigVec *vec = pawnvec_new_mg();
    int nonnull_cnt_start = count_nonnulls_in_world(w);

    // roll until all pawns will migrate, check migvec
    for (int i = 0; i<w->pawn_arr_len; i++) {
        if (w->pawns2d[i]) {
            int cur_len = vec->len;
            while (vec->len == cur_len) {
                world_find_migrating_pawns(w,w->pawns2d[i],vec);
            }
        }
    }

    // ensure all pawn addresses in world are still there after migration
    void **add_buff_start = calloc(w->pawn_arr_len, sizeof(void*));
    void **add_buff_end = calloc(w->pawn_arr_len, sizeof(void*));

    load_addresses_pawns(w, add_buff_start);
    world_migrate_pawns(w, vec);
    load_addresses_pawns(w, add_buff_end);
    int check_cnt = check_pawn_addresses(add_buff_start, add_buff_end, w->pawn_arr_len);
    TEST_ASSERT_EQUAL_INT32(w->pawn_cnt, check_cnt);

    // check for additional pawns
    int nonnull_cnt_end = count_nonnulls_in_world(w);
    TEST_ASSERT_EQUAL_INT32(nonnull_cnt_start, nonnull_cnt_end);

    world_free(w);
    pawnvec_free_mg(vec);
    free(add_buff_start);
    free(add_buff_end);
    
}


static void clear_world(World *w) {
    for (int i =0; i<w->pawn_arr_len; i++) {
        pawn_free(w->pawns2d[i]);
        w->pawns2d[i] = NULL;
    }
}

void test_function_world_count_pawns_in_ring(void) {
    int e = 0;
    World *w = world_new(&e, 30, 30);

    // test surrounded case
    Point2d pnts[] = {
        {5,5},  // anchor
        {4,4},  // neighbors..
        {5,4},
        {6,4},
        {4,5},
        {6,5},
        {4,6},
        {5,6},
        {6,6}
    };
    for (int i = 0; i<9; i++) {
        world_add_new_pawn_by_point(w, pnts[i]);
    }
    int radius = 1;
    int rad_cnt = world_count_pawns_in_ring(w,w->pawns2d[convert_2d_to_1d_idx(pnts[0].x, pnts[0].y, w->x_width)], radius);
    TEST_ASSERT_EQUAL_INT32(8, rad_cnt);
    clear_world(w);
    
    // check boundary case
    Point2d pnts2[] = {
        {0,0},  // anchor
        {1,0},  // neighbors..
        {0,1},
        {1,1}
    };
    for (int i = 0; i<4; i++) {
        world_add_new_pawn_by_point(w, pnts2[i]);
    }
    rad_cnt = world_count_pawns_in_ring(w,w->pawns2d[convert_2d_to_1d_idx(pnts2[0].x, pnts2[0].y, w->x_width)], radius);
    TEST_ASSERT_EQUAL_INT32(3, rad_cnt);
    clear_world(w);

    // check zero case
    Point2d pnt = {5,5};
    world_add_new_pawn_by_point(w, pnt);
    rad_cnt = world_count_pawns_in_ring(w,w->pawns2d[convert_2d_to_1d_idx(pnt.x, pnt.y, w->x_width)], radius);
    TEST_ASSERT_EQUAL_INT32(0, rad_cnt);

    world_free(w);
    
}


// static void clear_address_buffer(void **ad, int len) {
//     for (int i = 0; i<len; i++) {
//         ad[i] = NULL;
//     }
// }

// static int count_nonull_address_matches(void **ab1, void **ab2, int len) {
//     int match_cnt = 0;
//     for (int i = 0; i<len; i++) {
//         if (ab1[i] == NULL && ab2[i] == NULL) continue;
//         if (ab1[i] == ab2[i]) match_cnt++;
//     }
//     return match_cnt;
// }

// static int setup_world_with_surounded_pawns(World *w) {
//     int e = 0;
//     w = world_new(&e, 30,30);
//     if (e) exit(1);
//     Point2d pnts[] = {
//         {5,5},  // anchor
//         {4,4},  // neighbors..
//         {5,4},
//         {6,4},
//         {4,5},
//         {6,5},
//         {4,6},
//         {5,6},
//         {6,6}
//     };
//     for (int i = 0; i<9; i++) {
//         world_add_new_pawn_by_point(w, pnts[i]);
//     }
//     return convert_2d_to_1d_idx(pnts[0].x, pnts[0].y, w->x_width);
// }

void test_function_world_kill_pawns(void) {
    World *w = new_populated_test_world(30,30,50);
    PawnVec * dead_pawns = pawnvec_new();
    
    // check after all pawn death all nulls in array
    while (dead_pawns->len < 50) {
        for (int i = 0; i<w->pawn_arr_len; i++) {
            world_age_pawn(w, w->pawns2d[i]);
            world_kill_pawn(w, w->pawns2d[i], dead_pawns);
        }
        w->season++;
    }

    TEST_ASSERT_EQUAL_INT32(50, dead_pawns->len);

    pawnvec_free(dead_pawns);
    world_free(w);

}



static void add_existing_pawn(World* w, Pawn *p) {
    if (!p) return;
    w->pawns2d[convert_2d_to_1d_idx(p->x_pos, p->y_pos, w->x_width)] = p;
    w->pawn_cnt++;
    w->alive_pawns++;
}

static void setup_world_with_provided_pawns(World **w, PawnVec *ps, int len) {
    int e = 0;
    *w = world_new(&e, 30,30);
    if (e) exit(1);

    for (int i = 0; i<len; i++) {
        add_existing_pawn(*w, ps->ps[i]);
    }
}

void test_function_world_mating_factor_check(void) {

    World *w = NULL;
    PawnVec *ps = pawnvec_new();
    Pawn *p1 = pawn_new(0,5,5,0, false);
    Pawn *p2 = pawn_new(0,5,10,0, false);
    pawnvec_add(ps, p1);
    pawnvec_add(ps, p2);
    setup_world_with_provided_pawns(&w, ps, 2);

    // check mating true result
    p1->mating_factor = 100;
    p2->mating_factor = 100;
    bool mating_check_true = world_mating_factor_check(p1, p2);
    TEST_ASSERT_TRUE(mating_check_true);

    // check mating false result
    p1->mating_factor = 0;
    p2->mating_factor = 0;
    bool mating_check_false = world_mating_factor_check(p1, p2);
    TEST_ASSERT_TRUE(!mating_check_false);

    pawnvec_free(ps);
    world_free(w);
    
}


void test_function_world_fertility_check(void) {

    World *w = NULL;
    PawnVec *ps = pawnvec_new();
    Pawn *p1 = pawn_new(0,5,5,0, false);
    Pawn *p2 = pawn_new(0,5,10,0, false);
    pawnvec_add(ps, p1);
    pawnvec_add(ps, p2);
    setup_world_with_provided_pawns(&w, ps, 2);

    // check fertility true result
    p1->fertility_factor = 100;
    p2->fertility_factor = 100;
    bool fertility_check_true = world_fertility_check(p1, p2);
    TEST_ASSERT_TRUE(fertility_check_true);

    // check fertility false result
    p1->fertility_factor = 0;
    p2->fertility_factor = 0;
    bool fertility_check_false = world_fertility_check(p1, p2);
    TEST_ASSERT_TRUE(!fertility_check_false);

    pawnvec_free(ps);
    world_free(w);
}

void test_function_world_mate_check(void) {

    World *w = NULL;
    PawnVec *ps = pawnvec_new();
    Pawn *p1 = pawn_new(0,5,5,0, false);
    Pawn *p2 = pawn_new(0,5,10,0, false);
    pawnvec_add(ps, p1);
    pawnvec_add(ps, p2);
    setup_world_with_provided_pawns(&w, ps, 2);

    // check mating true result
    p1->fertile = true; p2->fertile = true;
    p1->fertility_factor = 100; p1->mating_factor = 100; p1->mating_radius = 20;
    p2->fertility_factor = 100; p2->mating_factor = 100; p2->mating_radius = 20;
    bool mate_check_true = world_mate_check(p1, p2);
    TEST_ASSERT_TRUE(mate_check_true);

    // check mating false result
    p1->fertility_factor = 0; p1->mating_factor = 0; p1->mating_radius = 1;
    p2->fertility_factor = 0; p2->mating_factor = 0; p2->mating_radius = 1;
    bool mate_check_false = world_mate_check(p1, p2);
    TEST_ASSERT_TRUE(!mate_check_false);

    pawnvec_free(ps);
    world_free(w);

}


void test_function_world_get_mate(void) {

    // check no mate, returns NULL
    World *w = NULL;
    PawnVec *ps = pawnvec_new();
    Pawn *p1 = pawn_new(0,5,5,0, false);
    p1->fertility_factor=100;
    p1->mating_factor = 100;
    p1->fertile = true;
    p1->mating_radius = 20;
    pawnvec_add(ps, p1);
    setup_world_with_provided_pawns(&w, ps, 2);
    Pawn *should_be_null = world_get_mate(w,p1);
    TEST_ASSERT_TRUE(should_be_null == NULL);

    // check expected pawn is return
    Pawn *p2 = pawn_new(0,5,10,0, false);
    p2->fertility_factor=100;
    p2->mating_factor = 100;
    p2->fertile = true;
    p2->mating_radius = 20;
    add_existing_pawn(w, p2);
    Pawn *should_be_p2 = world_get_mate(w,p1);
    TEST_ASSERT_TRUE(should_be_p2 == p2);


    pawnvec_free(ps);
    world_free(w);
}


void test_function_world_mate(void) {

    World *w = NULL;
    PawnVec *ps = pawnvec_new();
    Pawn *p1 = pawn_new(0,5,5,0, false);
    p1->fertility_factor=100;
    p1->mating_factor = 100;
    p1->fertile = true;
    p1->mating_radius = 20;
    pawnvec_add(ps, p1);

    Pawn *p2 = pawn_new(0,5,10,0, false);
    p2->fertility_factor=100;
    p2->mating_factor = 100;
    p2->fertile = true;
    p2->mating_radius = 20;
    pawnvec_add(ps, p2);
    setup_world_with_provided_pawns(&w, ps, 2);

    PawnVec *retVec = pawnvec_new();
    PawnVec *bornVec = pawnvec_new();
    world_mate(w, p1, retVec, bornVec);
    // should be one new pawn and two elements in retVec
    TEST_ASSERT_EQUAL_INT32(2, retVec->len);
    TEST_ASSERT_EQUAL_INT32(1, bornVec->len);
    
    // status of both pawns should be "mated"
    TEST_ASSERT_TRUE(p1->mated == true);
    TEST_ASSERT_TRUE(p2->mated == true);

    pawnvec_free(ps);
    pawnvec_free(retVec);
    pawnvec_free(bornVec);
    world_free(w);

}


void test_function_world_update(void) {
    World *w = new_populated_test_world(50, 50, 100);
    
    while(w->season < 100) {
        world_update(w);
        w->season++;
    }
    
    TEST_ASSERT_TRUE(w->born_pawns > 0);
    TEST_ASSERT_TRUE(w->migrated_pawns > 0);
    TEST_ASSERT_TRUE(w->old_age_death > 0);
    TEST_ASSERT_TRUE(w->starved_pawns > 0);

    world_free(w);
}




int main(void) {

    srand( time(NULL) );
    SetRandomSeed(time(NULL));
    
    UNITY_BEGIN();

    RUN_TEST(test_function_world_populate);
    RUN_TEST(test_function_world_add_new_pawn_by_point);
    RUN_TEST(test_function_world_age_pawn);
    RUN_TEST(test_function_world_find_migrating_pawns);
    RUN_TEST(test_function_world_migrate_pawns);
    RUN_TEST(test_function_world_count_pawns_in_ring);
    RUN_TEST(test_function_world_kill_pawns);
    RUN_TEST(test_function_world_mating_factor_check);
    RUN_TEST(test_function_world_fertility_check);
    RUN_TEST(test_function_world_mate_check);
    RUN_TEST(test_function_world_get_mate);
    RUN_TEST(test_function_world_mate);
    RUN_TEST(test_function_world_update);

    return UNITY_END();
}
