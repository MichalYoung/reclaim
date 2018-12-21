//
// Simple smoke test for arena allocation
//


#include "arena.h"
#include <stdio.h>

struct node {
    char *s;
    struct node *link;
};

void build_list() {
    arena space = new_arena(100); // Small enough to force multiple allocations
    struct node *head = NULL;
    for (int i=0; i < 100; ++i) {
        // printf("Appending a node\n");
        struct node* cell = arena_alloc(space, sizeof(struct node));
        cell->s = arena_strdup(space, "Go!");
        cell->link = head;
        head = cell;
    }
    struct node* cur = head;
    while(cur != NULL) {
        printf("%s\n", cur->s );
        cur = cur->link;
    }
    free_arena(space);
}

int main(int argc, char** argv) {
    build_list();
    return 0;
}