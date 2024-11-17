/*
    test the various util functions
*/

#include <unity/unity.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/util.h"


void setUp(void) {
    /* This is run before EACH TEST */
}

void tearDown(void) {}


void test_function_string_to_int(void) {
    int err;
    //case: empty -> err == -1
    string_to_int("", &err);
    TEST_ASSERT_EQUAL_INT32(-1, err);

    //case: invalid char -> err == -2
    string_to_int("1o3", &err);
    TEST_ASSERT_EQUAL_INT32(-2, err);

    //case: out of bounds -> err == -3
    string_to_int("1000000000000000", &err);
    TEST_ASSERT_EQUAL_INT32(-3, err);

    string_to_int("-1000000000000000", &err);
    TEST_ASSERT_EQUAL_INT32(-3, err);

    //check output
    int res = string_to_int("3141592", &err);
    TEST_ASSERT_EQUAL_INT32(3141592, res);

    res = string_to_int("-3141592", &err);
    TEST_ASSERT_EQUAL_INT32(-3141592, res);
}


static int sum_array_ints(int *a, int len) {
    int sum = 0;
    for (int i = 0; i < len; i++) {
        sum += a[i];
    }
    return sum;
}

static void concat_number(char * s, int *a, int len) {
    char *p = s;
    for (int i = 0; i<len; i++) {
        *p = a[i] + '0';
        p++;
    }
    *p = '\0';
}

void test_function_shuffle_ints(void) {
    // 1.3076744e+12
    int tst[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    char buffer_start[32];
    concat_number(buffer_start, tst, 15);

    int sum = sum_array_ints(tst, 15);
    shuffle_ints(tst, 15);
    int sum_s = sum_array_ints(tst, 15);

    // ensure sum is constant
    TEST_ASSERT_EQUAL_INT32(sum, sum_s);

    // test sum after many shuffles
    for (int i = 0; i<100; i++) {
        shuffle_ints(tst, 15);
    }
    sum_s = sum_array_ints(tst, 15);
    TEST_ASSERT_EQUAL_INT32(sum, sum_s);

    char buffer_end[32];
    concat_number(buffer_end, tst, 15);
    //check that array are diff chance for same: 1:1.3076744e+12
    TEST_ASSERT_TRUE(strcmp(buffer_start, buffer_end) != 0);

}


static int check_vec_elements(Vec2d *v1, Vec2d *v2, int len) {
    int matches = 0;
    for (int i = 0; i<len; i++) {
        for (int j = 0; j<len; j++) {
            if (v1[i].x == v2[j].x && v1[i].y == v2[j].y) {
                matches++;
            }
        }
    }
    return matches;     // this should be equal to len
}

static int check_vec_positions(Vec2d *v1, Vec2d *v2, int len) {
    int pos_match = 0;
    for (int i = 0; i<len; i++) {
        if (v1[i].x == v2[i].x && v1[i].y == v2[i].y) {
            pos_match++;
        }
    }
    return pos_match;   // should be less than len
}

void test_function_shuffle_vecs(void) {
    int len = 20;
    Vec2d vecs[len];
    for (int i = 0; i<len; i++) {
        vecs[i].x = i;
        vecs[i].y = len-i;
    }

    Vec2d vecs_test[len];
    for (int i = 0; i < len; i++) {
        vecs_test[i] = vecs[i];
    }
    shuffle_vecs(vecs_test, len);

    // all vectors are still present and not dupped
    int matches = check_vec_elements(vecs, vecs_test, len);
    TEST_ASSERT_EQUAL_INT32(len, matches);

    // not equal (false negative: one in len!)
    TEST_ASSERT_TRUE(check_vec_positions(vecs, vecs_test, len) < len);

}


static bool vecs_include_zero_zero(Vec2d *vs, int len) {
    for (int i = 0; i<len; i++) {
        if (vs[i].x == 0 && vs[i].y==0) return true;
    }
    return false;
}

static int vecs_not_in_order(Vec2d *vs, int rad) {
    int ordered_cnt = 0;

    int id = 0;
    for (int row = -rad; row <= rad; row++) {
        for (int col = -rad; col <= rad; col++) {
            if (row == 0 && col == 0) continue;
            
            if(vs[id].x == col && vs[id].y == row) ordered_cnt++;
            id++;
        }
    }

    return ordered_cnt;
}

void test_function_generate_random_offsets(void) {
    

    // negative or zero radius returns NULL
    Vec2d *tst_vec = generate_random_offsets(-10);
    TEST_ASSERT_TRUE(tst_vec == NULL);
    free(tst_vec);

    tst_vec = generate_random_offsets(0);
    TEST_ASSERT_TRUE(tst_vec == NULL);
    free(tst_vec);

    // result set does not include V(0,0)
    int rad = 10;
    int len = (2 * rad + 1) * (2 * rad + 1) -1;
    tst_vec = generate_random_offsets(rad);
    TEST_ASSERT_TRUE(vecs_include_zero_zero(tst_vec, len) == false);

    // results are not sequential
    TEST_ASSERT_TRUE(vecs_not_in_order(tst_vec, rad) < len);

    free(tst_vec);

}   


void test_function_convert_2d_to_1d_idx(void) {

    //case (2,2) in width 100 -> 2 * 100 + 2 = 202
    TEST_ASSERT_TRUE(convert_2d_to_1d_idx(2,2,100) == 202);

    // negative values of x or y return -1
    TEST_ASSERT_EQUAL_INT32(-1, convert_2d_to_1d_idx(-1,10,10));
    TEST_ASSERT_EQUAL_INT32(-1, convert_2d_to_1d_idx(10,-1,10));
    // w <= 0 returns -1
    TEST_ASSERT_EQUAL_INT32(-1, convert_2d_to_1d_idx(10,10,0));
    // x <= w returns -1
    TEST_ASSERT_EQUAL_INT32(-1, convert_2d_to_1d_idx(10,10,10));

}

static bool point_eq(Point2d p1, Point2d p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

void test_function_convert_1d_to_2d_idx(void) {
    
    // case idx 11 in 5 x 5 array (1,2)
    TEST_ASSERT_TRUE(point_eq(convert_1d_to_2d_idx(11,5), (Point2d){1,2}));
    // case negative x
    TEST_ASSERT_TRUE(point_eq(convert_1d_to_2d_idx(-5,5), (Point2d){-1,-1}));
    // case window <= 0
    TEST_ASSERT_TRUE(point_eq(convert_1d_to_2d_idx(12,0), (Point2d){-1,-1}));

}


static bool vecs_inside_mag(Vec2d *vs, int len, int mag) {
    for (int i = 0; i<len; i++) {
        if (vs[i].x > mag || vs[i].y > mag) return false;
    }
    return true;
}


static bool vec_eq(Vec2d v1, Vec2d v2) {
    return v1.x == v2.x && v1.y == v2.y;
}

void test_function_generate_random_vector(void) {
    
    // ensure vector is inside of passed magnitude
    int mag = 5, len = 20;
    Vec2d vecs[len];
    for (int i = 0; i<len; i++) {
        vecs[i] = generate_random_vector(mag);
    }

    TEST_ASSERT_TRUE(vecs_inside_mag(vecs,len,mag));

    // check when mag is < 1 <0,0> returned
    TEST_ASSERT_TRUE(vec_eq(generate_random_vector(0), (Vec2d){0,0}));
}


int main(void) {

    srand( time(NULL) );
    
    UNITY_BEGIN();

    RUN_TEST(test_function_string_to_int);
    RUN_TEST(test_function_shuffle_ints);
    RUN_TEST(test_function_shuffle_vecs);
    RUN_TEST(test_function_generate_random_offsets);
    RUN_TEST(test_function_convert_2d_to_1d_idx);
    RUN_TEST(test_function_convert_1d_to_2d_idx);
    RUN_TEST(test_function_generate_random_vector);

    return UNITY_END();
}
