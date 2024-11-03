
#ifndef UTIL_H_
#define UTIL_H_

/*  convert a string (from command line, or elsewhere) into an int
    handle errors by setting negative values in a passed error pointer:
        -1 nothing to parse
        -2 not an int
        -3 out of bounds
*/
int string_to_int(const char*, int*);


#endif