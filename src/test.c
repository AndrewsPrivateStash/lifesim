// let's test our linked list methods and make sure they work


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/plist.h"
#include "../include/pawn.h"
#include "../include/world.h"
#include "../include/util.h"


int main(void) {

    const int n = 2;

    Point2d *pnts = generate_random_offsets(n);
    
    int cnt = (n*2+1) * (n*2+1) - 1;
    for (int i = 0; i<cnt; i++) {
        printf("%d:\t(%d, %d)\n", i, pnts[i].x, pnts[i].y);
    }


    free(pnts);

    return 0;
}
