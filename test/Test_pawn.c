/*
    test the various pawn functions
*/

#include <unity/unity.h>
#include <raylib.h>
#include <time.h>
#include "../include/pawn.h"


void setUp(void) {
    /* This is run before EACH TEST */
}

void tearDown(void) {}


void test_function_pawn_new(void) {

    Pawn *tst = pawn_new(0,0,0,0,false);

    // fields populated
    TEST_ASSERT_TRUE(tst->id == 0);
    TEST_ASSERT_TRUE(tst->alive == true);
    TEST_ASSERT_TRUE(tst->bday == 0);
    TEST_ASSERT_TRUE(tst->x_pos == 0);
    TEST_ASSERT_TRUE(tst->y_pos == 0);
    TEST_ASSERT_TRUE(tst->age == 0);
    TEST_ASSERT_TRUE(tst->fertile == false);
    TEST_ASSERT_TRUE(tst->mated == false);
    
    pawn_free(tst);

}


void test_function_pawn_age(void) {

    Pawn *tst = pawn_new(0,0,0,0,false);

    int age = tst->age;
    pawn_age(tst);
    TEST_ASSERT_TRUE(tst->fertile==false);

    // verify age increment
    TEST_ASSERT_EQUAL_INT32(age+1,tst->age);

    // check fertility flips
    tst->age = 13;
    pawn_age(tst);
    TEST_ASSERT_TRUE(tst->fertile==true);
    tst->gen_age = 100;
    while (tst->age < 90) {
        pawn_age(tst);
    }
    TEST_ASSERT_TRUE(tst->fertile==false);

    pawn_free(tst);
    
}


int main(void) {

    SetRandomSeed(time(NULL));
    
    UNITY_BEGIN();

    RUN_TEST(test_function_pawn_new);
    RUN_TEST(test_function_pawn_age);


    return UNITY_END();
}
