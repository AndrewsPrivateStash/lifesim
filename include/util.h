
#ifndef UTIL_H_
#define UTIL_H_

/*  convert a string (from command line, or elsewhere) into an int
    handle errors by setting negative values in a passed error pointer:
        -1 nothing to parse
        -2 not an int
        -3 out of bounds
*/
int string_to_int(const char*, int*);

// swap two int in an array
void swap_ints(int*, int*);

// shuffle int array
void shuffle_ints(int*, int);

// print an array
void print_array(void*, int, char);

// print int matrix
void print_int_matrix(int*, int, int);


#endif