/*
    defines config.ini ingestion and storage

    config grammar
    identifier -> value
    value -> atomic
    value -> [value, value,...]
    # comment

    if identifier is not contained in the Config struct then ignore and emmit to stderr
    if value cannot be parsed into expected datatype then ignore and emit to stderr

*/


#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdint.h>
#include "clist.h"


enum ConType {
    INT_ = 1,
    FLOAT_,
    INTARRAY_
};
typedef enum ConType ConType;

enum ConTokType {
    IDENT_,
    VAL_,
    COMMENT_
};
typedef enum ConTokType ConTokType;

typedef struct ConfToken {
    uint16_t seq;   // label value tokens will have same seq (comments == 0)
    ConTokType type;
    char tok[64];
} ConfToken;

typedef struct ConfigElem {
    char name[64];
    ConType type;
} ConfigElem;

typedef struct LabelValue {
    char label[64];
    void *value;
} LabelValue;




typedef struct Config {
    // main constants
    int screenWidth;                                // screeen width in pxs
    int screenHeight;                               // screen height in pxs

    // Pawn constants
    int _GEN_AGE_LOWER_BND;                         // min genetic age of Pawn
    int _GEN_AGE_UPPER_BND;                         // max genetic age of Pawn
    int _FERT_FACT_LOWER_BND;                       // min fertility factor of Pawn
    int _FERT_FACT_UPPER_BND;                       // max fertility factor of Pawn
    int _MATING_RADIUS_LOWER_BND;                   // min mating radius of Pawn
    int _MATING_RADIUS_UPPER_BND;                   // max mating radius of Pawn
    int _MATING_FACTOR_LOWER_BND;                   // min mating factor of Pawn
    int _MATING_FACTOR_UPPER_BND;                   // max mating factor of Pawn
    double _FERTILITY_DECAY;                        // the decay factor applied to fertility factor each season
    int _FERTILITY_START;                           // the age a Pawn becomes fertile

    // World constants
    int _WIN_WIDTH_OFFSET;                          // pixel offset from window width boundary for pawn population
    int _WIN_HEIGHT_OFFSET;                         // pixel offset from window height boundary for pawn population
    int _PAWN_SEARCH_RADIUS;                        // pixel radius to search around mid-point of parents
    int _PAWN_MAX_POSSIBLE_MATES;                   // the maximum number of possible mates a pawn can store in it's radius
    int *_PAWN_STARVE_PROBS;                        // array of prob of pawn starving based on immediate ring population
    int *_PAWN_ATTACKED_PROBS;                      // array of prob of pawn being attacked based on immediate ring population
    int _PAWN_MIGRATION_RADIUS;                     // max radius a pawn can migrate
    int _PAWN_MIGRATION_PROB;                       // chance pawn migrates in a season
    int _PAWN_MIGRATION_PROB_DENOM;                 // chance pawn migrates in a season
    int _PAWN_RING_RADIUS;                          // radius around pawn to check for other pawns (for starve, and attack rolls)

    // General
    int cells_in_ring;                              // the count of cells in ring radius (2r+1)^2-1
    int pause_on_start;                             // does the sim start paused (0,1)

} Config;


// initialize config object with default values prior to file parsing
Config *config_init(void);

// free Config opbject and associated memory
void config_free(Config*);

// load file
char *config_load_file(Config*, const char*);


typedef struct Clist Clist; // forward declare

// tokenize file
Clist *config_tokenize(char*);

// parse tokens into Config
void config_parse(Config*, Clist*);

// print config object to a stream
void config_print(Config*, FILE*);

// load the config struct
void config_load(Config *, const char *);


#endif