
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>


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


void swap_ints(int *a, int *b) {
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}


void shuffle_ints(int *a, int len) {
    srand ( time(NULL) );   // randomize seed

    int j;
    for (int i = len -1; i > 0; i--) {
        j = rand() % (i + 1);
        swap_ints(&a[j], &a[i]);
    }
}


void print_array(void *a, int len, char type) {
    // types: 'i': int, 'c': chars

    switch (type) {
        case 'i':
        {
            printf("\n[");
            for (int i = 0; i<len-1; i++) {
                printf("%d,", *((int*)a + i));
            }
            printf("%d]\n", *((int*)a + len - 1));
        
        break;
        }

        case 'c':
        {
            printf("\n[");
            for (int i = 0; i<len-1; i++) {
                printf("%c,", *((char*)a + i));
            }
            printf("%c]\n", *((char*)a + len - 1));
        
        break;
        }

    }

}


void print_int_matrix(int* mat, int c, int r) {

    printf("\n");
    for (int row = 0; row < r; row++) {
        printf("| ");
        for (int col = 0; col < c; col++) {
            printf("%d ", mat[row * c + col]);
        }
        printf("|\n");
    }
    printf("\n");

}