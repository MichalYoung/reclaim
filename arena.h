//
// Alternative allocation/free strategy, suitable for structures
// that are built up with a limited lifetime and which can be
// guaranteed not to "escape" outside that lifetime.   An arena
// is a pool of storage from which we make incremental allocations
// and then, when done, free all at once.
//

#ifndef AST_ARENA_H
#define AST_ARENA_H

struct arena_chunk;

struct arena_header {
    struct arena_chunk *current_chunk;
    char *magic;
};

typedef struct arena_header* arena;

/* Create an arena for allocation */
arena new_arena(int size_in_bytes);

/* Allocate a new object of n bytes within arena */
void * arena_alloc(arena the_arena, int bytes);

/* Since strndup and strncat would create pointers
 * into unmanaged space, we need our own version.
 * Simple versions (no limit on length) because these
 * are into our managed arena.
 */
char *arena_strdup(arena where, char *s);
char *arena_strcat(arena where, char *s, char *t);

/* Reclaim all the objects in the arena. */
void free_arena(arena the_arena);

#endif //AST_ARENA_H
