/* config source */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "../include/util.h"
#include "../include/clist.h"
#include "../include/config.h"

#define MAP_INT(x) c->x = *(int*)(lbv.value)
#define MAP_FLOAT(x) c->x = *(double*)(lbv.value)
#define MAP_INT_ARRAY(x) c->x = (int*)(lbv.value)


static int ERR = 0; // local error int
Config *CONFIG = NULL;

// defines the element types for parsing
static const ConfigElem ELEMS[] = {
    {"screenWidth", INT_},
    {"screenHeight", INT_},
    {"_GEN_AGE_LOWER_BND", INT_},
    {"_GEN_AGE_UPPER_BND", INT_},
    {"_FERT_FACT_LOWER_BND", INT_},
    {"_FERT_FACT_UPPER_BND", INT_},
    {"_MATING_RADIUS_LOWER_BND", INT_},
    {"_MATING_RADIUS_UPPER_BND", INT_},
    {"_MATING_FACTOR_LOWER_BND", INT_},
    {"_MATING_FACTOR_UPPER_BND", INT_},
    {"_FERTILITY_DECAY", FLOAT_},
    {"_FERTILITY_START", INT_},
    {"_WIN_WIDTH_OFFSET", INT_},
    {"_WIN_HEIGHT_OFFSET", INT_},
    {"_PAWN_SEARCH_RADIUS", INT_},
    {"_PAWN_MAX_POSSIBLE_MATES", INT_},
    {"_PAWN_RING_RADIUS", INT_},
    {"cells_in_ring", INT_},
    {"_PAWN_STARVE_PROBS", INTARRAY_},
    {"_PAWN_ATTACKED_PROBS", INTARRAY_},
    {"_PAWN_MIGRATION_RADIUS", INT_},
    {"_PAWN_MIGRATION_PROB", INT_},
    {"_PAWN_MIGRATION_PROB_DENOM", INT_}

};
static const int CONFIG_LEN = sizeof(ELEMS) / sizeof(ELEMS[0]);

static void print_int_array_to_stream(FILE*, int*, int);

Config *config_init(void) {
    Config *c = malloc(sizeof(Config));

    if (!c) return NULL;

    c->screenWidth = 1000;
    c->screenHeight = 800;

    c->_GEN_AGE_LOWER_BND = 1;
    c->_GEN_AGE_UPPER_BND = 80;
    c->_FERT_FACT_LOWER_BND = 0;
    c->_FERT_FACT_UPPER_BND = 100;
    c->_MATING_RADIUS_LOWER_BND = 1;
    c->_MATING_RADIUS_UPPER_BND = 10;
    c->_MATING_FACTOR_LOWER_BND = 0;
    c->_MATING_FACTOR_UPPER_BND = 100;
    c->_FERTILITY_DECAY = 0.95;
    c->_FERTILITY_START = 13;

    c->_WIN_WIDTH_OFFSET = 10;
    c->_WIN_HEIGHT_OFFSET = 10;
    c->_PAWN_SEARCH_RADIUS = 5;
    c->_PAWN_MAX_POSSIBLE_MATES = 3;
    c->_PAWN_RING_RADIUS = 1;
    c->cells_in_ring = (2 * c->_PAWN_RING_RADIUS + 1) * (2 * c->_PAWN_RING_RADIUS + 1) -1;
    
    c->_PAWN_STARVE_PROBS = calloc(8, sizeof(int));
    int starve_default[8] = {0, 0, 0, 2, 3, 5, 10, 15};
    for (int i = 0; i<8; i++) {
        c->_PAWN_STARVE_PROBS[i] = starve_default[i];
    }

    c->_PAWN_ATTACKED_PROBS = calloc(8, sizeof(int));   
    int attack_default[] = {0, 0, 0, 0, 1, 2, 3, 5};
    for (int i = 0; i<8; i++) {
        c->_PAWN_ATTACKED_PROBS[i] = attack_default[i];
    }

    c->_PAWN_MIGRATION_RADIUS = 20;
    c->_PAWN_MIGRATION_PROB = 75;
    c->_PAWN_MIGRATION_PROB_DENOM = 1000;
    
    return c;
}


void config_free(Config *c) {
    free(c->_PAWN_STARVE_PROBS);
    free(c->_PAWN_ATTACKED_PROBS);
    free(c);
}


void config_print(Config *c, FILE *stream) {

    if (!c || !stream) return;

    fprintf(stream, "\n########## LIFESIM CONFIG ##########\n\n");
    fprintf(stream, "screenWidth: %d\n",c->screenWidth);
    fprintf(stream, "screenHeight: %d\n",c->screenHeight);

    fprintf(stream, "_GEN_AGE_LOWER_BND: %d\n",c->_GEN_AGE_LOWER_BND);
    fprintf(stream, "_GEN_AGE_UPPER_BND: %d\n",c->_GEN_AGE_UPPER_BND);
    fprintf(stream, "_FERT_FACT_LOWER_BND: %d\n",c->_FERT_FACT_LOWER_BND);
    fprintf(stream, "_FERT_FACT_UPPER_BND: %d\n",c->_FERT_FACT_UPPER_BND);
    fprintf(stream, "_MATING_RADIUS_LOWER_BND: %d\n",c->_MATING_RADIUS_LOWER_BND);
    fprintf(stream, "_MATING_RADIUS_UPPER_BND: %d\n",c->_MATING_RADIUS_UPPER_BND);
    fprintf(stream, "_MATING_FACTOR_LOWER_BND: %d\n",c->_MATING_FACTOR_LOWER_BND);
    fprintf(stream, "_MATING_FACTOR_UPPER_BND: %d\n",c->_MATING_FACTOR_UPPER_BND);
    fprintf(stream, "_FERTILITY_DECAY: %f\n",c->_FERTILITY_DECAY);
    fprintf(stream, "_FERTILITY_START: %d\n",c->_FERTILITY_START);

    fprintf(stream, "_WIN_WIDTH_OFFSET: %d\n",c->_WIN_WIDTH_OFFSET);
    fprintf(stream, "_WIN_HEIGHT_OFFSET: %d\n",c->_WIN_HEIGHT_OFFSET);
    fprintf(stream, "_PAWN_SEARCH_RADIUS: %d\n",c->_PAWN_SEARCH_RADIUS);
    fprintf(stream, "_PAWN_MAX_POSSIBLE_MATES: %d\n",c->_PAWN_MAX_POSSIBLE_MATES);
    fprintf(stream, "_PAWN_STARVE_PROBS: ");
    print_int_array_to_stream(stream, c->_PAWN_STARVE_PROBS, c->cells_in_ring);
    fprintf(stream, "_PAWN_ATTACKED_PROBS: ");
    print_int_array_to_stream(stream, c->_PAWN_ATTACKED_PROBS, c->cells_in_ring);
    fprintf(stream, "_PAWN_MIGRATION_RADIUS: %d\n",c->_PAWN_MIGRATION_RADIUS);
    fprintf(stream, "_PAWN_MIGRATION_PROB: %d\n",c->_PAWN_MIGRATION_PROB);
    fprintf(stream, "_PAWN_MIGRATION_PROB_DENOM: %d\n",c->_PAWN_MIGRATION_PROB_DENOM);

    fprintf(stream, "_PAWN_RING_RADIUS: %d\n",c->_PAWN_RING_RADIUS);
    fprintf(stream, "cells_in_ring: %d\n",c->cells_in_ring);
    fprintf(stream, "\n####################################\n");
    
}


static void print_int_array_to_stream(FILE *stream, int *a, int len) {
        
    fprintf(stream, "[");
    for (int i = 0; i<len-1; i++) {
        fprintf(stream, "%d,", a[i]);
    }
    fprintf(stream, "%d]\n", a[len - 1]);

}

static long config_get_file_size(FILE *f) {
    long file_size = 0;
    fseek(f, 0L, SEEK_END);
    file_size = ftell(f);
    fseek(f, 0L, SEEK_SET);
    return file_size;
}

char *config_load_file(Config *c, const char *file_path) {

    FILE *f = NULL;
    if (access(file_path, F_OK) != 0) {
        // file does not exist
        f = fopen(file_path, "w");
        if (!f) {
            fprintf(stderr, "problem opening config file to write at: %s\n", file_path);
            exit(1);
        }
        config_print(c, f);
        fclose(f);
    }

    f = fopen(file_path, "r+");
    if (!f) {
        fprintf(stderr, "could not open config file for reading: %s\n", file_path);
    }

    // Get file size
    long file_size = config_get_file_size(f);
    if (file_size == 0) {
        config_print(c, f);
        file_size = config_get_file_size(f);
    };

    char *raw_file = calloc(file_size + 1, sizeof(char));
    if (!raw_file) {
        fprintf(stderr, "failed to allocate config file buffer\n");
        fclose(f);
        exit(1);
    }

    //read file into buffer
    if ( fread(raw_file, sizeof(char), file_size, f) == 0 ) {
        fprintf(stderr, "error reading config file: %s\n", file_path);
        fclose(f);
        free(raw_file);
        exit(1);
    }

    fclose(f);
    return raw_file;

}


 Clist *config_tokenize(char *raw_file) {
    // break the file into tokens

    if (!raw_file) return (Clist*)0;
    Clist *lst = clist_new_list();
    char buff[256] = {0};
    uint16_t seq = 1;
    // proceed char by char
    char *ptr = raw_file;
    while(*ptr != '\0') {

        // case \n
        if (*ptr == '\n') {
            ptr++;
            continue;
        }

        // case white space
        if (isspace(*ptr)) {
            ptr++;
            continue;
        }

        // start of a line

        // case comment
        if (*ptr == '#') {
            if (*(ptr+1) != '\0') ptr++;

            int i = 0;
            while (*ptr != '\n' && *ptr != '\0') {
                buff[i] = *ptr;
                i++;
                ptr++;
            }
            buff[i] = '\0';
            ConfToken t = {.seq=0, .type = COMMENT_};
            strcpy(t.tok, buff);
            clist_push_back(lst, t);
            continue;
        }

        // case IDENT: VAL
        if (isalpha(*ptr) || *ptr == '_') {

            int i = 0;
            while (*ptr != ':' && *ptr != '\0') {
                buff[i] = *ptr;
                i++;
                ptr++;
            }

            // at colon
            buff[i] = '\0';
            ConfToken tid = {.seq=seq, .type = IDENT_};
            strcpy(tid.tok, buff);
            clist_push_back(lst, tid);
            if (*ptr == '\0') break;    // this shouldn't happen, obviously

            // skip whitespace
            ptr++;
            if (*ptr == '\0') break;
            for(;isspace(*ptr); ptr++);

            // at VAL
            if (*ptr == '\0') break;
            i = 0;
            while (*ptr != '\n' && *ptr != '\0') {
                if (isspace(*ptr)) {
                    ptr++;
                    continue;
                }
                buff[i] = *ptr;
                i++;
                ptr++;
            }
            buff[i] = '\0';
            ConfToken tv = {.seq=seq, .type = VAL_};
            strcpy(tv.tok, buff);
            clist_push_back(lst, tv);
            seq++;
            if (*ptr == '\0') break;
            ptr++;

        }

    }

    return lst;

}


static ConType get_type(ConfToken t) {
    for (int i = 0; i < CONFIG_LEN; i++) {
        if (strcmp(t.tok, ELEMS[i].name) == 0) {
            return ELEMS[i].type;
        }
    }
    return (ConType)0;  // identifier not found
}

static double parse_double(const char *s, int *e) {
    char *end_ptr;
    double out = strtod(s, &end_ptr);    // returns 0.00 if not a valid float
    if (s == end_ptr) {
        *e = -1;    // bad parse
    }
    return out;
}

static int *parse_int_array(char *s, int *len, int *e) {
    // take in token of array vals and return a pointer to a constructed array
    // length is set to the passed int pointer for checking against config later
    if (!s) {
        *e = -1;
        return (int*)0;
    }

    int *out_arr = malloc(sizeof(*out_arr));
    if (!out_arr) {
        fprintf(stderr, "failed to allocated array in config construction\n");
        exit(1);
    }
    uint8_t arr_idx = 0;


    char buff[8] = {0}; // store multi digit int strings
    int insert_int;
    while (*s) {
        if ( strchr("\n[],", *s) || isspace(*s) ) {
            s++;
            continue;
        }

        // digits might be more than 1 char, so concat
        if ( isdigit(*s) ) {
            int i = 0;
            while ( isdigit(*s) ) {
                buff[i] = *s;
                i++; s++;
            }
            buff[i] = '\0';
            insert_int = string_to_int(buff, &ERR);
            if (ERR) {
                fprintf(stderr, "failed to convert string to int for: %s\n", buff);
                exit(1);
            }
            if (arr_idx == 0) {
                out_arr[arr_idx] = insert_int;
                arr_idx++;
            } else {
                out_arr = realloc(out_arr, sizeof(*out_arr) * (arr_idx + 1) + 1);
                out_arr[arr_idx] = insert_int;
                arr_idx++;
            }
            
        }
        s++;

    }
    *len = arr_idx;
    return out_arr;

}

static void map_token_to_config(Config *c, LabelValue lbv) {
    
    // if else ladder of destiny
    // this is super manual, I wonder if there is a better way to do this..
    // maybe write a config maper that takes a function pointer for mapping the values
    
    if (strcmp(lbv.label, "screenWidth") == 0) {MAP_INT(screenWidth);}
    else if (strcmp(lbv.label, "screenHeight") == 0) {MAP_INT(screenHeight);}
    else if (strcmp(lbv.label, "_GEN_AGE_LOWER_BND") == 0) {MAP_INT(_GEN_AGE_LOWER_BND);}
    else if (strcmp(lbv.label, "_GEN_AGE_UPPER_BND") == 0) {MAP_INT(_GEN_AGE_UPPER_BND);}
    else if (strcmp(lbv.label, "_FERT_FACT_LOWER_BND") == 0) {MAP_INT(_FERT_FACT_LOWER_BND);}
    else if (strcmp(lbv.label, "_FERT_FACT_UPPER_BND") == 0) {MAP_INT(_FERT_FACT_UPPER_BND);}
    else if (strcmp(lbv.label, "_MATING_RADIUS_LOWER_BND") == 0) {MAP_INT(_MATING_RADIUS_LOWER_BND);}
    else if (strcmp(lbv.label, "_MATING_RADIUS_UPPER_BND") == 0) {MAP_INT(_MATING_RADIUS_UPPER_BND);}
    else if (strcmp(lbv.label, "_MATING_FACTOR_LOWER_BND") == 0) {MAP_INT(_MATING_FACTOR_LOWER_BND);}
    else if (strcmp(lbv.label, "_MATING_FACTOR_UPPER_BND") == 0) {MAP_INT(_MATING_FACTOR_UPPER_BND);}
    else if (strcmp(lbv.label, "_FERTILITY_DECAY") == 0) {MAP_FLOAT(_FERTILITY_DECAY);}
    else if (strcmp(lbv.label, "_FERTILITY_START") == 0) {MAP_INT(_FERTILITY_START);}
    else if (strcmp(lbv.label, "_WIN_WIDTH_OFFSET") == 0) {MAP_INT(_WIN_WIDTH_OFFSET);}
    else if (strcmp(lbv.label, "_WIN_HEIGHT_OFFSET") == 0) {MAP_INT(_WIN_HEIGHT_OFFSET);}
    else if (strcmp(lbv.label, "_PAWN_SEARCH_RADIUS") == 0) {MAP_INT(_PAWN_SEARCH_RADIUS);}
    else if (strcmp(lbv.label, "_PAWN_MAX_POSSIBLE_MATES") == 0) {MAP_INT(_PAWN_MAX_POSSIBLE_MATES);}
    else if (strcmp(lbv.label, "_PAWN_RING_RADIUS") == 0) {MAP_INT(_PAWN_RING_RADIUS);}
    else if (strcmp(lbv.label, "cells_in_ring") == 0) {MAP_INT(cells_in_ring);}
    else if (strcmp(lbv.label, "_PAWN_STARVE_PROBS") == 0) {
        free(c->_PAWN_STARVE_PROBS);
        MAP_INT_ARRAY(_PAWN_STARVE_PROBS);
    }
    else if (strcmp(lbv.label, "_PAWN_ATTACKED_PROBS") == 0) {
        free(c->_PAWN_ATTACKED_PROBS);
        MAP_INT_ARRAY(_PAWN_ATTACKED_PROBS);
    }
    else if (strcmp(lbv.label, "_PAWN_MIGRATION_RADIUS") == 0) {MAP_INT(_PAWN_MIGRATION_RADIUS);}
    else if (strcmp(lbv.label, "_PAWN_MIGRATION_PROB") == 0) {MAP_INT(_PAWN_MIGRATION_PROB);}
    else if (strcmp(lbv.label, "_PAWN_MIGRATION_PROB_DENOM") == 0) {MAP_INT(_PAWN_MIGRATION_PROB_DENOM);}
    
}


void config_parse(Config *c, Clist *lst) {
    // take in the token list and parse the tokens into the config struct
    if (!lst) return;
    if (!lst->head) return;

    ConfToken cur, nxt;     // cur should be ident and nxt the val
    ConType cur_type;
    while (lst->cnt > 0) {
        // empty list
        cur = clist_pop(lst);
        if (cur.type == COMMENT_) continue;

        cur = clist_pop(lst);
        nxt = clist_pop(lst);

        // incomplete token pair
        if (cur.seq != nxt.seq) {
            fprintf(stderr, "%s, has seq: %u; doesn't match %s, with seq: %u\n",
                cur.tok, cur.seq, nxt.tok, nxt.seq
            );
            clist_push(lst, nxt);
            continue;
        }

        // make sure first token is identifier
        if (cur.type != IDENT_) {
            fprintf(stderr, "malformed label value pair, first token not identifier\n");
            fprintf(stderr, "1st token: %s, 2nd token: %s\n", cur.tok, nxt.tok);
            clist_push(lst, nxt);
            continue;
        }

        // find the type for the val
        cur_type = get_type(cur);
        if (!cur_type) {
            fprintf(stderr, "%s not a recognized identifier\n", cur.tok);
            clist_push(lst, nxt);
            continue;
        }

        LabelValue lbv;
        strcpy(lbv.label, cur.tok);

        //parse val
        switch (cur_type) {
            case INT_:  {
                int tmp = string_to_int(nxt.tok, &ERR);
                if (ERR) {
                    fprintf(stderr, "error parsing %s, skipping\n", nxt.tok);
                    ERR = 0;
                    continue;
                }
                lbv.value = &tmp;
                break;
            }
            case FLOAT_: {
                double tmp = parse_double(nxt.tok, &ERR);
                if (ERR) {
                    fprintf(stderr, "error parsing %s, skipping\n", nxt.tok);
                    ERR = 0;
                    continue;
                }
                lbv.value = &tmp;
                break;
            }
            case INTARRAY_: {
                int prob_len = 0;
                int *tmp = parse_int_array(nxt.tok, &prob_len, &ERR);
                if (ERR) {
                    fprintf(stderr, "error parsing %s, skipping\n", nxt.tok);
                    ERR = 0;
                    continue;
                }
                lbv.value = tmp;
                break;
            }
            default: {
                fprintf(stderr, "did not recognize type for: %s\n", nxt.tok);
                continue;
            }
        }

        // send LabelValue to mapping function to load struct
        map_token_to_config(c, lbv);
        assert(c->cells_in_ring == (2 * c->_PAWN_RING_RADIUS + 1) * (2 * c->_PAWN_RING_RADIUS + 1) -1);

    }
    
}


void config_load(Config *c, const char *path) {
    char *raw_file = config_load_file(c, path);
    Clist *lst = config_tokenize(raw_file);
    config_parse(c, lst);
    clist_free(lst);
    free(raw_file);
}