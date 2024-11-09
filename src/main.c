/*
    graphical life sim

    use raylib
    https://www.raylib.com/

    - Take a set of initial conditions for a population of a race
    - simulate the lifecycle of generations moving through time
    - taking into account scarcity, proximity, threats, reproduction, etc

    - visually represent the population evolve over time as colored pixels in the world map, as generations progress

    ToDO:
        - make sure all pawns are placed in the beginning
        - optimize mate function use better data structures

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "raylib.h"         // https://github.com/raysan5/raylib/tree/master
#include "../include/pawn.h"
#include "../include/world.h"
#include "../include/util.h"


int main(int argc, char **argv) {
    // Initialization
    //--------------------------------------------------------------------------------------

    // ############# Initial Conditions #############
    int starting_population = 100;  // how many little dudes do you start with (first arg from cml)


    // ##### commandline args #####

    // ensure there is no more than 1 argument
    if (argc > 2) {
        fprintf(stderr, "lifesim expects zero or one argument, the starting population as an int (def:100)\n$lifesim n\n");
        exit(1);
    }

    if (argc == 2) {
        int err = 0;
        starting_population =  string_to_int(argv[1], &err);
        if (err) {
            fprintf(stderr, "%s could not be parsed as an int\nERRCODE:%d\n", argv[1], err);
            exit(1);
        }

        if (starting_population <= 0) {
            fprintf(stderr, "%d starting population is less than 1, which doesn't make sense\n", starting_population);
            exit(1);
        }
    }


    // ########### Initialization ###########
    
    const int screenWidth = 400;
    const int screenHeight = 400;

    const int season_x_pos = screenWidth - 150;
    const int season_y_pos = 0;
    const int pawn_x_pos = 10;
    const int pawn_y_pos = 0;

    // generate world and populate with starting Pawns
    SetRandomSeed(time(NULL));

    int err = 0;
    World *world = world_new(&err);
    if (err) {
        fprintf(stderr, "error allocating world struct\n");
        exit(1);
    }

    world_populate(world, screenWidth, screenHeight, starting_population);
    world_get_all_mates(world); // populate the mates lists
    printf("world populated with %u Pawns\n", world->pawn_cnt);
    
    char title_buffer[128], season_buffer[128], pawn_buffer[128];

    if (snprintf(title_buffer, sizeof(title_buffer), "Lifesim (%d Pawns)", world->pawn_cnt) <= 0) {
        strncpy(title_buffer, "Lifesim", sizeof(title_buffer)-1);
    }
    
    // ###################### TEST ######################
    /*
    int pwn_cnt=0;
    int counter = 0;
    while (counter<20) {
        if (snprintf(season_buffer, sizeof(season_buffer), "Season: %u", world->season) <= 0) {
            strncpy(season_buffer, "Season: NA", sizeof(season_buffer)-1);
        }
        printf("Season Buffer: %s\n", season_buffer);

        if (snprintf(pawn_buffer, sizeof(pawn_buffer), "Pawns: %u", world->alive_pawns) <= 0) {
            strncpy(pawn_buffer, "Pawns: NA", sizeof(pawn_buffer)-1);
        }
        printf("pawn buffer: %s\n", pawn_buffer);

        pwn_cnt = 0;        
        for (int i = 0; i < world->pawn_cnt; i++) {
            if (world->pawns[i]->alive) {
                pwn_cnt++;
            }
        }
        if (pwn_cnt != world->alive_pawns) printf("world->alive_pawns: %u should be: %d\n", world->alive_pawns, pwn_cnt);

        // mate the Pawns
        world_mate(world);

        // age the pawns (who were not just born)
        world_age_pawns(world);
        world_kill_pawns(world);
        (world->season)++;

        for (int i =0; i < world->pawn_cnt; i++) {
            pawn_print(world->pawns[i]);
        }

        // printf("ENTER for next loop\n");
        // getchar();
        counter++;

    }

    return 0;
    */
    // ###################### END TEST ######################




    InitWindow(screenWidth, screenHeight, title_buffer);
    SetTargetFPS(5);

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {        

        (world->season)++;

        // Draw
        BeginDrawing();

            ClearBackground((Color){63, 59, 57, 255});

            // counters ***************************************
            if (snprintf(season_buffer, sizeof(season_buffer), "Season: %u", world->season) <= 0) {
               strncpy(season_buffer, "Season: NA", sizeof(season_buffer)-1);
            }
            DrawText(season_buffer, season_x_pos, season_y_pos, 25, (Color){0, 196, 122, 200});

            if (snprintf(pawn_buffer, sizeof(pawn_buffer), "Pawns: %u", world->alive_pawns) <= 0) {
               strncpy(pawn_buffer, "Pawns: NA", sizeof(pawn_buffer)-1);
            }
            DrawText(pawn_buffer, pawn_x_pos, pawn_y_pos, 25, (Color){0, 196, 122, 200});

            // ************************************************

            for (int i = 0; i < world->pawn_cnt; i++) {
                if (world->pawns[i]->alive) {
                    DrawPixel(world->pawns[i]->x_pos, world->pawns[i]->y_pos, (Color){71, 110, 233, 255});
                }
            }

        EndDrawing();

        // ############### UPDATE STATE ###############

        world_mate(world);              // mate the Pawns

        world_age_pawns(world);         // age the pawns (who were not just born)
        world_kill_pawns(world);        // retire dead pawns
        world_purge_mates(world);       // remove dead or infertile pawns from lists
        world_get_all_mates(world);     // update mating lists

        if (world->alive_pawns == 0) break;
        
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    

    // add terminal dump of final stats on close
    world_free(world);
    return 0;
}