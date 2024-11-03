
#include <stdlib.h>
#include <limits.h>


int string_to_int(const char *str, int *err){
    char *endptr;
    long int num;
    num = strtol(str, &endptr, 10);
    

    if (endptr == str){     //empty string
        *err = -1;
        return 0;
    }

    if (*endptr != '\0'){   //invalid char, so pointer did not stop on null char
        *err = -2;
        return 0;
    }

    if(num > INT_MAX || num < INT_MIN){ //value is not inside the INT bounds
        *err = -3;
        return 0;
    }

    return (int) num;
}