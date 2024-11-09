// let's test our linked list methods and make sure they work


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/plist.h"
#include "../include/pawn.h"
#include "../include/world.h"


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

int main(void) {

    int *arr = calloc(100, sizeof(int));

    for (int i = 0; i<20; i++) {
        arr[i] = 1;
    }

    shuffle_ints(arr, 100);

    char c_arr[5] = "abcd";

    print_array(arr, 100, 'i');
    print_array(&c_arr, 5, 'c');

    free(arr);


    /*

    int e = 0;
    World *w = world_new(&e);
    world_populate(w, 200, 200, 25, 10);


    // populate the lists
    world_get_all_mates(w);
    

    for (int i = 0; i < w->pawn_cnt; i++) {
        pawn_print(w->pawns[i]);
        plist_print(w->pawns[i]->possible_mates);
        puts("\n");
    }

    world_free(w);
    */

    return 0;
}
