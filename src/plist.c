/* NOT USED CURRENTLY */

#include "../include/plist.h"
#include <stdio.h>


void plist_add_node(Plist *list, Pawn *p, unsigned short d) {
    if (!p) {   // pawn required
        return;
    }
    
    if (list->head) { // list exists
        
        Pnode *cur = list->head;
        while (cur->next) {     //treverse list
            cur = cur->next;
        }

        Pnode *new_node = malloc(sizeof(Pnode));
        if (!new_node) {
            fprintf(stderr, "error allocating memory for pawn at: %p (%d,%d)\n", p, p->x_pos, p->y_pos);
            exit(1);
        }
        cur->next = new_node;   // assigned new ellement to end

        // populate node
        new_node->pwn = p;
        new_node->dist = d;
        new_node->next = NULL;
        
        // update list
        list->cnt++;

    } else {    // empty list, add first element
        Pnode *new_node = malloc(sizeof(Pnode));
        if (!new_node) {
            fprintf(stderr, "error allocating memory for pawn at: %p (%d,%d)\n", p, p->x_pos, p->y_pos);
            exit(1);
        }

        // populate node
        new_node->pwn = p;
        new_node->dist = d;
        new_node->next = NULL;

        // update list
        list->cnt++;
        list->head = new_node;

    }
}


void plist_remove_node(Plist *list, Pawn *p) {
    if (!list || !list->head || !p) {    // nothing to do
        return;
    }

    // pawn at head
    if (list->head->pwn == p) {
        Pnode *temp = list->head;
        list->head = list->head->next;  // reassign head

        free(temp);
        return;
    } 

    Pnode *cur = list->head;
    Pnode *prev = NULL;
    while (cur->pwn != p && cur->next) {    // advance to pawn, or end
        prev = cur;
        cur = cur->next;
    }

    // pawn not found
    if (!cur->next && cur->pwn != p) return;

    // pawn at end
    if (!cur->next) {
        prev->next = NULL;

        free(cur);
        return;
    }

    // pawn in middle
    prev->next = cur->next; // reassign the next pointer of previous
    free(cur);
    
}


bool plist_inlist(Plist *list, Pawn *p) {
    if(!list || !list->head || !p) return false;

    Pnode *cur = list->head;
    while (cur) {
        if (cur->pwn == p) return true;
        cur = cur->next;
    }
    return false;
}


void plist_purge(Plist *list) {
    if (!list || !list->head) return;

    Pnode *cur = list->head;
    Pnode *nxt = NULL;

    while (cur) {

        if (!cur->pwn->alive || !cur->pwn->fertile) {
            nxt = cur->next;
            plist_remove_node(list, cur->pwn);
            cur = nxt;
            continue;
        }
        cur = cur->next;

    }
}


void plist_print(Plist* list) {
    if (!list) return;
    if (!list->head) return;

    Pnode *cur = list->head;
    while (cur) {
        printf("owning: (%d,%d)\tpossible: (%d,%d)\t%u\n", list->owning_pawn->x_pos, list->owning_pawn->y_pos,cur->pwn->x_pos, cur->pwn->y_pos, cur->dist);
        cur = cur->next;
    }
}


void plist_free(Plist *list) {
    if (!list) return;
    if (!list->head) return;

    Pnode *cur = list->head;
    Pnode *prev;
    while (cur) {
        prev = cur;
        cur = cur->next;
        free(prev);
    }
    free(list);
}

