// let's test our linked list methods and make sure they work


#include <stdio.h>
#include "../include/plist.h"
#include "../include/pawn.h"
#include "../include/world.h"



int main(void) {

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


    return 0;
}
