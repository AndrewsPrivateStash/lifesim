
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "../include/util.h"


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

void swap_pnts(Point2d *a, Point2d *b) {
    Point2d tmp;
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

void shuffle_points(Point2d *a, int len) {
    srand ( time(NULL) );   // randomize seed

    int j;
    for (int i = len -1; i > 0; i--) {
        j = rand() % (i + 1);
        swap_pnts(&a[j], &a[i]);
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


Point2d *generate_random_offsets(int rad) {
    if (rad < 1) return NULL;

    // given radius r, grid has side len: 2r+1 => (2r+1)^2 -1 results, excluding (0,0) offset
    int pnt_cnt = (2 * rad + 1) * (2 * rad + 1);
    Point2d *pnts = malloc( (pnt_cnt -1) * sizeof(Point2d) );


    // make vectors
    int id = 0;
    for (int row = -rad; row <= rad; row++) {
        for (int col = -rad; col <= rad; col++) {
            if (row == 0 && col == 0) continue; // skip trivial (0,0) offset
            pnts[id].x = col;
            pnts[id].y = row;
            id++;
        }
    }

    // shuffle points
    shuffle_points(pnts, pnt_cnt -1);

    return pnts;

}