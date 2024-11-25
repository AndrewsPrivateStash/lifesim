/* Stack and Queue list for config tokens */

#include "../include/clist.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


Clist *clist_new_list(void) {
    Clist *lst = malloc(sizeof(*lst));
    if (!lst) {
        return (Clist*)0;
    }
    lst->head = NULL;
    lst->tail = NULL;
    lst->cnt = 0;

    return lst;
}


static void free_node(Cnode *n) {
    free(n->token);
    free(n);
}

void clist_free(Clist *lst) {
    if (!lst) return;
    if (!lst->head) {
        free(lst);
        return;
    }

    Cnode *cur = lst->head;
    Cnode *prev = NULL;
    while (cur) {
        prev = cur;
        cur = cur->next;
        free_node(prev);
    }
    free(lst);
}

static void copy_tkn_to_node(Cnode *n, ConfToken t) {
    if (!n) return;
    n->token->seq = t.seq;
    n->token->type = t.type;
    strcpy(n->token->tok, t.tok);
}

static ConfToken copy_node_to_tkn(Cnode *n) {
    if (!n) return (ConfToken){};
    ConfToken out_tok = {
        .seq = n->token->seq,
        .type = n->token->type,
    };
    strcpy(out_tok.tok, n->token->tok);

    return out_tok;
    
}

static Cnode *make_new_node(void) {
    Cnode *new_node = malloc(sizeof(*new_node));
    if (!new_node) return (Cnode*)0;
    
    new_node->token = malloc(sizeof(ConfToken));
    if (!new_node->token) {
        free(new_node);
        return (Cnode*)0;
    }
    new_node->next = NULL;
    return new_node;

}


// ############ Stack operations ############

void clist_push(Clist *lst, ConfToken c) {
    // push a node to the head of the list
    if (!lst) return;
    Cnode *new_node = make_new_node();
    if (!new_node) {
        fprintf(stderr, "failed to allocate new clist node\n");
        exit(1);
    }
    copy_tkn_to_node(new_node, c);

    new_node->next = lst->head;
    lst->head = new_node;
    if (lst->cnt == 0) {
        lst->tail = new_node;
    }
    lst->cnt++;
}


ConfToken clist_pop(Clist *lst) {
    if (!lst) return (ConfToken){};
    if (!lst->head) return (ConfToken){};

    // pop the head node from the list
    ConfToken out_tok = copy_node_to_tkn(lst->head);
    Cnode *rem = lst->head;
    lst->head = lst->head->next;    // NULLs the head if last element
    if (lst->tail == rem) {
        lst->tail = NULL;
    }
    lst->cnt--;
    free_node(rem);

    return out_tok;

}


// ############ Queue operations ############

void clist_push_back(Clist *lst, ConfToken c) {
    // push a node to the tail of the list
    if (!lst) return;
    Cnode *new_node = make_new_node();
    if (!new_node) {
        fprintf(stderr, "failed to allocate new clist node\n");
        exit(1);
    }
    copy_tkn_to_node(new_node, c);

    if (!lst->tail) {
        lst->tail = new_node;
        lst->head = new_node;
    } else {
        lst->tail->next = new_node;
        lst->tail = new_node;
    }

    lst->cnt++;

}


ConfToken clist_pop_back(Clist *lst) {
    if (!lst) return (ConfToken){};
    if (!lst->head) return (ConfToken){};

    // pop the tail node from the list
    ConfToken out_tok = copy_node_to_tkn(lst->tail);
    Cnode *rem = lst->tail;

    // case only one element, so: head == tail
    if (lst->head == rem) {
        lst->head = NULL;
        lst->tail = NULL;

    } else {
    // case more than one element, so head != tail

        // move to second to last node
        Cnode *curser = lst->head;
        while (curser->next != rem) {
            curser = curser->next;
        }
        curser->next = NULL;
    }

    lst->cnt--;
    free_node(rem);

    return out_tok;

}


// ############ General ############


void clist_print(Clist *lst, FILE *f) {
    if (!lst) return;
    if (!lst->head) return;

    Cnode *node = lst->head;
    char tok_type[32] = {0};
    fprintf(f, "clist: %p, cnt: %u, head: %p, tail: %p\n\n", lst, lst->cnt, lst->head, lst->tail);
    while(node) {
        fprintf(f, "node: %p\n", node);
        fprintf(f, "seq: %u,  ", node->token->seq);
        switch (node->token->type) {
            case 0:
                strcpy(tok_type, "IDENT_");
                break;
            case 1:
                strcpy(tok_type, "VAL_");
                break;
            case 2:
                strcpy(tok_type, "COMMENT_");
                break;
            default:
                strcpy(tok_type, "UNKNOWN");
        }
        fprintf(f, "type: %s,  ", tok_type);
        fprintf(f, "tok: %s\n", node->token->tok);
        fprintf(f, "next: %p\n\n", node->next);
        node = node->next;
    }

}

