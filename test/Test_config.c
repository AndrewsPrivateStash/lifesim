/*
    test the various config functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <unity/unity.h>
#include "../include/util.h"
#include "../include/config.h"


void setUp(void) {
    /* This is run before EACH TEST */
}

void tearDown(void) {}


void test_function_config_init(void) {

    Config *c = config_init();

    // object created
    TEST_ASSERT_NOT_NULL(c);
    
    config_free(c);

}

void test_function_config_print(void) {

    Config *c = config_init();

    // print output for manual inspection
    config_print(c, stdout);
    TEST_PASS();    

    config_free(c);

}

void test_function_config_load_file(void) {

    Config *c = config_init();

    char *raw_file = config_load_file(c, "config.ini");
    printf("%s", raw_file);
    TEST_PASS();
    free(raw_file);

}

void test_function_config_tokenize(void) {

    Config *c = config_init();
    char *raw_file = config_load_file(c, "config.ini");
    Clist *lst = config_tokenize(raw_file);
    clist_print(lst, stdout);

    TEST_PASS();

    config_free(c);
    clist_free(lst);
    
}

void test_function_config_parse(void) {

    Config *c = config_init();
    char *raw_file = config_load_file(c, "config.ini");
    Clist *lst = config_tokenize(raw_file);
    config_parse(c, lst);
    config_print(c, stdout);
    
    config_free(c);
    clist_free(lst);

    TEST_PASS();
       
}





int main(void) {

    
    UNITY_BEGIN();

    RUN_TEST(test_function_config_init);
    RUN_TEST(test_function_config_print);
    RUN_TEST(test_function_config_load_file);
    RUN_TEST(test_function_config_tokenize);
    RUN_TEST(test_function_config_parse);


    return UNITY_END();
}