//
// Created by Michal Young on 2018-12-18.
//

#include "arena.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* We allocate chunks of memory from the underlying
 * malloc/free system in chunks several
 * times larger than the largest object we will
 * allocate, to keep wasted space limited.
 */

struct cell { void *a_word; };

struct arena_chunk {
    struct arena_chunk *next_chunk;
    int words_capacity;
    int words_used;
    struct cell mem[];
};

static char* MAGIC = "Arena";


#define PTR_SIZE  sizeof(void *)
#define words(s) ((s + PTR_SIZE - 1) / PTR_SIZE)

void get_another_chunk(arena the_arena, int size_in_bytes);

/* Create an arena for allocation */
arena new_arena(int size_in_bytes) {
    arena the_arena = (arena) malloc(sizeof(struct arena_header));
    the_arena->current_chunk = NULL;
    the_arena->magic = MAGIC;
    get_another_chunk(the_arena, size_in_bytes);
    return the_arena;
}

void get_another_chunk(arena the_arena, int size_in_bytes) {
    int n_words = words(size_in_bytes);
    struct arena_chunk* the_chunk = (struct arena_chunk*) malloc(sizeof(struct arena_chunk) + sizeof(void *[n_words]));
    the_chunk->next_chunk = the_arena->current_chunk;
    the_arena->current_chunk = the_chunk;
    the_chunk->words_used = 0;
    the_chunk->words_capacity = n_words;
}

/* Allocate a new object of n bytes within arena */
void * arena_alloc(arena the_arena, int bytes) {
    int words_needed = words(bytes);
    if (the_arena->current_chunk->words_capacity - the_arena->current_chunk->words_used < words_needed) {
        get_another_chunk(the_arena, bytes);
    }
    struct cell *chunk_begin = &(the_arena->current_chunk->mem[the_arena->current_chunk->words_used]);
    the_arena->current_chunk->words_used += words_needed;
    return (void *) chunk_begin;
}

char *arena_strdup(arena where, char *s) {
    size_t len = strlen(s) + 1;
    assert(words(len) < where->current_chunk->words_capacity);
    char *space = (char *) arena_alloc(where, len);
    char *dest = space;
    while (*s != '\0') {
        *dest = *s;
        ++dest; ++s;
    }
    *dest = *s;
    return space;
}

char *arena_strcat(arena where, char *s, char *t) {
    size_t len = strlen(s) + strlen(t) + 1;
    assert(words(len) < where->current_chunk->words_capacity);
    char *space = (char *) arena_alloc(where, len);
    char *dest = space;
    while (*s != '\0') {
        *dest = *s;
        ++dest; ++s;
    }
    while (*t != '\0') {
        *dest = *t;
        ++dest; ++t;

    }
    *dest = '\0';
    return space;
}


void reclaim_chunk_recursive(struct arena_chunk *chunk) {
    if (chunk == NULL) return;
    reclaim_chunk_recursive(chunk->next_chunk);
    free(chunk);
}

/* Reclaim all the objects in the arena. */
void free_arena(arena the_arena) {
    reclaim_chunk_recursive(the_arena->current_chunk);
    free(the_arena);
}




