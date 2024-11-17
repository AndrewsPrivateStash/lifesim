/*
    test the various pawnvec functions
*/

#include <unity/unity.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/pawnvec.h"


static const int _STARTING_CAP = 10;

void setUp(void) {
    /* This is run before EACH TEST */
}

void tearDown(void) {}


void test_function_pawnvec_new(void) {

    // initial capacity and length check
    PawnVec *tstvec = pawnvec_new();
    TEST_ASSERT_EQUAL_INT32(_STARTING_CAP, tstvec->cap);
    TEST_ASSERT_EQUAL_INT32(0, tstvec->len);

    pawnvec_free(tstvec);

}


static PawnVec* make_pawnvec(int len) {
    PawnVec *tmp = pawnvec_new();
    for (int i = 0; i<len; i++) {
        tmp->ps[i] = pawn_new(i, 0, 0, 0, false);
        tmp->len++;
    }
    return tmp;
}

static void free_pawnvec(PawnVec *pv) {
    for (int i = 0; i<pv->len; i++) {
        pawn_free(pv->ps[i]);
    }
    pawnvec_free(pv);
}

static void load_addresses(PawnVec *pv, void **ab) {
    ab[0] = (void*) pv;
    for (int i = 0; i<pv->len; i++) {
        ab[i+1] = (void*) pv->ps[i];
    }
}

static int compare_addresses(void **ab1, void **ab2, int len) {
    int match_cnt = 0;
    for (int i = 0; i<len+1; i++) {
        printf("ad1: %p\tad2: %p\n", ab1[i], ab2[i]);
        if (ab1[i] == ab2[i]) match_cnt++;
    }
    return match_cnt;
}

void test_function_pawnvec_resize(void) {
    
    PawnVec *tst = make_pawnvec(_STARTING_CAP-1);
    int starting_cap = tst->cap, starting_len = tst->len;

    // capacity and len
    // case: pawnvec doesn't need to be resized
    pawnvec_resize(tst);
    TEST_ASSERT_EQUAL_INT32(starting_cap, tst->cap);
    TEST_ASSERT_EQUAL_INT32(starting_len, tst->len);
    free_pawnvec(tst);

    // capacity doubles
    tst = make_pawnvec(_STARTING_CAP);
    starting_cap = tst->cap, starting_len = tst->len;
    void *address_buffer_start[128], *address_buffer_after[128];
    load_addresses(tst, address_buffer_start);
    pawnvec_resize(tst);
    load_addresses(tst, address_buffer_after);

    TEST_ASSERT_EQUAL_INT32(_STARTING_CAP * 2, tst->cap);
    
    // check addresses
    int cnt = compare_addresses(address_buffer_start, address_buffer_after, tst->len);
    TEST_ASSERT_EQUAL_INT32(tst->len+1, cnt);   // pointer to vector, and each pawn pointer
    free_pawnvec(tst);
}


void test_function_pawnvec_add(void) {

    PawnVec *tst = make_pawnvec(_STARTING_CAP-1);
    int starting_cap = tst->cap, starting_len = tst->len;
    void *address_buffer_start[128], *address_buffer_after[128];
    load_addresses(tst, address_buffer_start);

    // add one inside of cap
    pawnvec_add(tst, pawn_new(tst->len, 0,0,0,false));
    load_addresses(tst, address_buffer_after);
    int cnt = compare_addresses(address_buffer_start, address_buffer_after, tst->len-1);

    TEST_ASSERT_EQUAL_INT32(starting_cap, tst->cap);
    TEST_ASSERT_EQUAL_INT32(starting_len + 1, tst->len);
    TEST_ASSERT_EQUAL_INT32(tst->len, cnt);   // pointer to vector, and each prior pawn pointer

    // add past cap boundary
    starting_cap = tst->cap, starting_len = tst->len;
    load_addresses(tst, address_buffer_start);
    pawnvec_add(tst, pawn_new(tst->len, 0,0,0,false));
    load_addresses(tst, address_buffer_after);
    cnt = compare_addresses(address_buffer_start, address_buffer_after, tst->len-1);

    TEST_ASSERT_EQUAL_INT32(starting_cap * 2, tst->cap);
    TEST_ASSERT_EQUAL_INT32(starting_len + 1, tst->len);
    TEST_ASSERT_EQUAL_INT32(tst->len, cnt);   // pointer to vector, and each prior pawn pointer

}

// ########### MIGVEC TESTS ###########

void test_function_pawnvec_new_mg(void) {

    // initial capacity and length check
    MigVec *tstvec = pawnvec_new_mg();
    TEST_ASSERT_EQUAL_INT32(_STARTING_CAP, tstvec->cap);
    TEST_ASSERT_EQUAL_INT32(0, tstvec->len);

    pawnvec_free_mg(tstvec);
}


void test_function_pawnvec_new_migpawn(void) {

    MigPawn *tst = pawnvec_new_migpawn(
        pawn_new(0,0,0,0,false),
        (Point2d){0,0}
    );

    TEST_ASSERT_TRUE(tst->p != NULL);
    TEST_ASSERT_TRUE(tst->mig_pnt.x == 0 && tst->mig_pnt.y == 0);

    pawn_free(tst->p);
    free(tst);
}


static MigVec* make_pawnvec_mg(int len) {
    MigVec *tmp = pawnvec_new_mg();
    for (int i = 0; i<len; i++) {
        pawnvec_add_mg(tmp,
            pawnvec_new_migpawn(
                pawn_new(i, 0,0,0,false),
                (Point2d){0,0}
            )
        );
    }
    return tmp;
}

static void free_pawnvec_mg(MigVec *pv) {
    for (int i = 0; i<pv->len; i++) {
        pawn_free(pv->ps[i]->p);
    }
    pawnvec_free_mg(pv);
}

static void load_addresses_mg(MigVec *pv, void **ab) {
    ab[0] = (void*) pv;
    for (int i = 0; i<pv->len; i++) {
        ab[i+1] = (void*) pv->ps[i]->p;
    }
}

void test_function_pawnvec_resize_mg(void) {

    MigVec *tst = make_pawnvec_mg(_STARTING_CAP-1);
    int starting_cap = tst->cap, starting_len = tst->len;

    // capacity and len
    // case: pawnvec doesn't need to be resized
    pawnvec_resize_mg(tst);
    TEST_ASSERT_EQUAL_INT32(starting_cap, tst->cap);
    TEST_ASSERT_EQUAL_INT32(starting_len, tst->len);
    free_pawnvec_mg(tst);

    // capacity doubles
    tst = make_pawnvec_mg(_STARTING_CAP);
    starting_cap = tst->cap, starting_len = tst->len;
    void *address_buffer_start[128], *address_buffer_after[128];
    load_addresses_mg(tst, address_buffer_start);
    pawnvec_resize_mg(tst);
    load_addresses_mg(tst, address_buffer_after);

    TEST_ASSERT_EQUAL_INT32(_STARTING_CAP * 2, tst->cap);
    
    // check addresses
    int cnt = compare_addresses(address_buffer_start, address_buffer_after, tst->len);
    TEST_ASSERT_EQUAL_INT32(tst->len+1, cnt);   // pointer to vector, and each pawn pointer
    free_pawnvec_mg(tst);

}


void test_function_pawnvec_add_mg(void) {

    MigVec *tst = make_pawnvec_mg(_STARTING_CAP-1);
    int starting_cap = tst->cap, starting_len = tst->len;
    void *address_buffer_start[128], *address_buffer_after[128];
    load_addresses_mg(tst, address_buffer_start);

    // add one inside of cap
    pawnvec_add_mg(tst, pawnvec_new_migpawn(
            pawn_new(tst->len, 0,0,0,false),
            (Point2d){0,0}
        ));
    load_addresses_mg(tst, address_buffer_after);
    int cnt = compare_addresses(address_buffer_start, address_buffer_after, tst->len-1);

    TEST_ASSERT_EQUAL_INT32(starting_cap, tst->cap);
    TEST_ASSERT_EQUAL_INT32(starting_len + 1, tst->len);
    TEST_ASSERT_EQUAL_INT32(tst->len, cnt);   // pointer to vector, and each prior pawn pointer

    // add past cap boundary
    starting_cap = tst->cap, starting_len = tst->len;
    load_addresses_mg(tst, address_buffer_start);
    pawnvec_add_mg(tst, pawnvec_new_migpawn(
            pawn_new(tst->len, 0,0,0,false),
            (Point2d){0,0}
        ));
    load_addresses_mg(tst, address_buffer_after);
    cnt = compare_addresses(address_buffer_start, address_buffer_after, tst->len-1);

    TEST_ASSERT_EQUAL_INT32(starting_cap * 2, tst->cap);
    TEST_ASSERT_EQUAL_INT32(starting_len + 1, tst->len);
    TEST_ASSERT_EQUAL_INT32(tst->len, cnt);   // pointer to vector, and each prior pawn pointer

    pawnvec_free_mg(tst);
}


void test_high_volume_migvect(void) {
    MigVec *tst = make_pawnvec_mg(5000);
    TEST_ASSERT_TRUE(tst != NULL);
    pawnvec_free_mg(tst);
}


int main(void) {

    srand( time(NULL) );
    
    UNITY_BEGIN();

    RUN_TEST(test_function_pawnvec_new);
    RUN_TEST(test_function_pawnvec_resize);
    RUN_TEST(test_function_pawnvec_add);

    RUN_TEST(test_function_pawnvec_new_mg);
    RUN_TEST(test_function_pawnvec_new_migpawn);
    RUN_TEST(test_function_pawnvec_resize_mg);
    RUN_TEST(test_function_pawnvec_add_mg);
    RUN_TEST(test_high_volume_migvect);

    return UNITY_END();
}
