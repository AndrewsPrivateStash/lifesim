/*
    test the various clist functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <unity/unity.h>
#include "../include/clist.h"


void setUp(void) {
    /* This is run before EACH TEST */
}

void tearDown(void) {}


void test_function_clist_new_list(void) {

    Clist *lst = clist_new_list();
    TEST_ASSERT_NOT_NULL(lst);
    TEST_ASSERT_EQUAL_UINT16(0, lst->cnt);
    TEST_ASSERT_NULL(lst->head);
    TEST_ASSERT_NULL(lst->tail);

    clist_free(lst);
}


void test_function_clist_push(void) {

    ConfToken toks[] = {
        (ConfToken){.seq = 1, .type = IDENT_, .tok = "A_NAME"},
        (ConfToken){.seq = 1, .type = VAL_, .tok = "123"},
        (ConfToken){.seq = 0, .type = COMMENT_, .tok = "#interesting comment"},  
    };
    
    Clist *lst = clist_new_list();
    for (int i = 0; i<3; i++) {
        clist_push(lst, toks[i]);
    }

    TEST_ASSERT_EQUAL_UINT16(3, lst->cnt);

    clist_free(lst);
}


void test_function_clist_pop(void) {

    ConfToken toks[] = {
        (ConfToken){.seq = 1, .type = IDENT_, .tok = "A_NAME"},
        (ConfToken){.seq = 1, .type = VAL_, .tok = "123"},
        (ConfToken){.seq = 0, .type = COMMENT_, .tok = "#interesting comment"},  
    };
    
    Clist *lst = clist_new_list();
    for (int i = 0; i<3; i++) {
        clist_push(lst, toks[i]);
    }
    Cnode *snd = lst->head->next;
    Cnode *thrd = lst->head->next->next;    // should be tail
    TEST_ASSERT_TRUE(thrd == lst->tail);

    ConfToken popped = clist_pop(lst);
    TEST_ASSERT_EQUAL_UINT16(0, popped.seq);
    TEST_ASSERT_EQUAL_INT32(2, popped.type);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("#interesting comment", popped.tok,19);
    TEST_ASSERT_EQUAL_UINT16(2, lst->cnt);
    TEST_ASSERT_EQUAL_PTR(snd, lst->head);
    TEST_ASSERT_TRUE(thrd == lst->tail);

    clist_free(lst);

}


void test_function_clist_push_back(void) {

    ConfToken toks[] = {
        (ConfToken){.seq = 1, .type = IDENT_, .tok = "A_NAME"},
        (ConfToken){.seq = 1, .type = VAL_, .tok = "123"},
        (ConfToken){.seq = 0, .type = COMMENT_, .tok = "#interesting comment"},  
    };
    
    Clist *lst = clist_new_list();
    for (int i = 0; i<3; i++) {
        clist_push_back(lst, toks[i]);
    }

    TEST_ASSERT_EQUAL_UINT16(3, lst->cnt);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("A_NAME", lst->head->token->tok, 6);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("#interesting comment", lst->tail->token->tok, 19);

    clist_free(lst);
}


void test_function_clist_pop_back(void) {

    ConfToken toks[] = {
        (ConfToken){.seq = 1, .type = IDENT_, .tok = "A_NAME"},
        (ConfToken){.seq = 1, .type = VAL_, .tok = "123"},
        (ConfToken){.seq = 0, .type = COMMENT_, .tok = "#interesting comment"},  
    };
    
    Clist *lst = clist_new_list();
    for (int i = 0; i<3; i++) {
        clist_push_back(lst, toks[i]);
    }
    

    ConfToken popped = clist_pop_back(lst);
    TEST_ASSERT_EQUAL_UINT16(0, popped.seq);
    TEST_ASSERT_EQUAL_INT32(2, popped.type);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("#interesting comment", popped.tok,19);
    TEST_ASSERT_EQUAL_UINT16(2, lst->cnt);
    

    clist_free(lst);

}




void test_function_clist_print(void) {

    ConfToken toks[] = {
        (ConfToken){.seq = 1, .type = IDENT_, .tok = "A_NAME"},
        (ConfToken){.seq = 1, .type = VAL_, .tok = "123"},
        (ConfToken){.seq = 0, .type = COMMENT_, .tok = "#interesting comment"},
        (ConfToken){.seq = 2, .type = IDENT_, .tok = "AN_ARRAY"},
        (ConfToken){.seq = 2, .type = VAL_, .tok = "[1, 2, 3, 4]"}, 
    };
    
    Clist *lst = clist_new_list();
    for (int i = 0; i<5; i++) {
        clist_push_back(lst, toks[i]);
    }

    clist_print(lst, stdout);
    TEST_PASS();

    clist_free(lst);

}
    



int main(void) {

    
    UNITY_BEGIN();

    RUN_TEST(test_function_clist_new_list);
    RUN_TEST(test_function_clist_push);
    RUN_TEST(test_function_clist_pop);
    RUN_TEST(test_function_clist_push_back);
    RUN_TEST(test_function_clist_pop_back);
    RUN_TEST(test_function_clist_print);

    return UNITY_END();
}