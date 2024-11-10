
#ifndef UTIL_H_
#define UTIL_H_


// store an xy coordinate
typedef struct Point2d {
    int x;
    int y;
} Point2d;


/*  convert a string (from command line, or elsewhere) into an int
    handle errors by setting negative values in a passed error pointer:
        -1 nothing to parse
        -2 not an int
        -3 out of bounds
*/
int string_to_int(const char*, int*);

// swap two int in an array
void swap_ints(int*, int*);

// swap two points
void swap_pnts(Point2d *, Point2d *);

// shuffle int array
void shuffle_ints(int*, int);

// shuffle points
void shuffle_points(Point2d *, int);

// print an array
void print_array(void*, int, char);

// print int matrix
void print_int_matrix(int*, int, int);

// generate random offsets excluding trivial (0,0) offset
Point2d *generate_random_offsets(int);


#endif