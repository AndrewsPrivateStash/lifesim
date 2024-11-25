
#ifndef UTIL_H_
#define UTIL_H_

#include <stdbool.h>

// store an xy coordinate
typedef struct Point2d {
    int x;
    int y;
} Point2d;

// a 2d int vector
typedef struct Vec2d {
    int x;
    int y;
} Vec2d;


/*  convert a string (from command line, or elsewhere) into an int
    handle errors by setting negative values in a passed error pointer:
        -1 nothing to parse
        -2 not an int
        -3 out of bounds
*/
int string_to_int(const char*, int*);

// check if a char is included in the string of chars
bool is_in_chars(const char*, const char*);

// swap two int in an array
void swap_ints(int*, int*);

// swap two points
void swap_vecs(Vec2d*, Vec2d *);

// shuffle int array
void shuffle_ints(int*, int);

// shuffle points
void shuffle_vecs(Vec2d*, int);

// print an array
void print_array(void*, int, char);

// print int matrix
void print_int_matrix(int*, int, int);

// generate random offsets excluding trivial (0,0) offset
Vec2d *generate_random_offsets(int);

// 2d to 1d index: y * w + x
int convert_2d_to_1d_idx(int x, int y, int w);

// 1d to 2d index: (i % w, i / w)
Point2d convert_1d_to_2d_idx(int i, int w);

// generate random vector
Vec2d generate_random_vector(int);



#endif