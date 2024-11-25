
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>
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

bool is_in_chars(const char *c, const char *ls) {
    while (ls) {
        if (*c == *ls) return true;
        ls++;
    }
    return false;
}


void swap_ints(int *a, int *b) {
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void swap_vecs(Vec2d *a, Vec2d *b) {
    Vec2d tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}


void shuffle_ints(int *a, int len) {
    // assumes srand( time(NULL) ) was already called

    int j;
    for (int i = len -1; i > 0; i--) {
        j = rand() % (i + 1);
        swap_ints(&a[j], &a[i]);
    }
}

void shuffle_vecs(Vec2d *a, int len) {
    // assumes srand( time(NULL) ) was already called

    int j;
    for (int i = len -1; i > 0; i--) {
        j = rand() % (i + 1);
        swap_vecs(&a[j], &a[i]);
    }
}


void print_array(void *a, int len, char type) {
    // types: 'i': int, 'c': chars

    switch (type) {
        case 'i':
        {
            printf("[");
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
        default:

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


Vec2d *generate_random_offsets(int rad) {
    if (rad < 1) return NULL;

    // given radius r, grid has side len: 2r+1 => (2r+1)^2 -1 results, excluding (0,0) offset
    int pnt_cnt = (2 * rad + 1) * (2 * rad + 1);
    Vec2d *pnts = malloc( (pnt_cnt -1) * sizeof(Vec2d) );   // caller frees


    // make offsets
    int id = 0;
    for (int row = -rad; row <= rad; row++) {
        for (int col = -rad; col <= rad; col++) {
            if (row == 0 && col == 0) continue; // skip trivial (0,0) offset
            pnts[id].x = col;
            pnts[id].y = row;
            id++;
        }
    }

    // shuffle offsets
    shuffle_vecs(pnts, pnt_cnt -1);

    return pnts;

}


int convert_2d_to_1d_idx(int x, int y, int w) {
    if (x < 0 || y < 0 || w < 0) return -1;
    if (x >= w ) return -1;
    return (y * w) + x;
}


Point2d convert_1d_to_2d_idx(int i, int w) {
    if (w < 1 || i < 0) return (Point2d){-1,-1};
    return (Point2d){ .x  = i % w , .y = i / w };
}


Vec2d generate_random_vector(int mag) {
    // assumes srand( time(NULL) ) was already called
    if (mag < 1) return (Vec2d){.x=0, .y=0};
    
    Vec2d out_vec;
    out_vec.x = rand() % (2 * mag + 1) - mag;
    out_vec.y = rand() % (2 * mag + 1) - mag;

    return out_vec;

}