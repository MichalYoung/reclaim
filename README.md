# reclaim

Was called "refcount" and supported only reference counting.  Now has two memory management alternatives. 

## Reference counting
Reference counted structures in ISO C  (experimental). 
Work in progress.  Initial version is working, but needs serious work in API as well as implementation. 

See Jones, Hosking and Moss, *The Garbage Collection Handbook*, CRC Press 2012 for an extensive analysis of reference counting implementations.  The current implementation is what GCH refers to as "the naive algorithm."  It is quite expensive in particular because we are changing counts as *roots* (local variables) are changed. 

Reference counting could best be used for structures of relatively large objects with fairly stable connections.  It is appropriate for buffers in the cache.  It is quite possibly appropriate for some tables and other standard ADTs.  

### To Do

I believe the reference count approach will never be the technique of choice for an AST or another structure with a large number of small objects and many traversals that don't change the basic connectivity of the structure.  However, it can be suited for some purposes.  

The most important change needed is that we need to 
reduce the number of updates for what I'll call *ephemeral* pointers.  As GCH notes, the main cost of reference counting is from changes in roots (e.g, consider a local variable that points to the current element in a linked list traversal).  An *ephemeral* pointer, which would include many but not all root pointers, would be a local variable that can be disregarded with respect to garbage collection, either because it is held only momentarily or because it is guaranteed not to be the only pointer to a structure. 

## Arenas

Arenas are based on the basic scheme described in 
Hanson, David R. (1989),  "Fast allocation and deallocation of memory based on object lifetimes". Software: Practice and Experience. 20 (1): 5–12. doi:10.1002/spe.4380200104.  (It is a simple scheme and I am implementing it from memory for now.  I will re-read Hanson later and may revise the design based on that.) 

The idea of arenas is that sometimes we can just deallocate many related objects together, greatly reducing the overhead of tracking each individual object.  Allocating in arenas can be cheaper than naked malloc/free, because there is no header information on individual objects. 

The API is as follows: 

### Create an arena
```arena new_arena(int size_in_bytes);```

The size given is the size of chunk that will be 
allocated as needed.  It should be large enough to 
hold many objects --- a sizeable fraction of the total 
space needed for the objects that will be allocated in the arena.  For example, if you wanted to create 100 objects of around 40 bytes each, the arena should be at least 1000 bytes (so there might be 4-6 underlying mallocs of memory chunks). 

Example: ```arena space = new_arena(4096);```

### Allocate an object
```void * arena_alloc(arena the_arena, int bytes);```

Essentially identical to ```malloc``` except that memory will be taken from the arena.  Another underlying malloc may take place if needed. 

Example: ```struct node* cell = arena_alloc(space, sizeof(struct node));```

### Copy or concatenate a string into an arena

If we want a char* in an object, we need it to be in the arena.  strdup calls malloc, so if we set a field to a strdup'd string, it will not be reclaimed with the arena.  We provide alternatives for copying strings into the arena. 

```char *arena_strdup(arena where, char *s);```
Copy a string into the arena. 

```char *arena_strcat(arena where, char *s, char *t);```
Copy a concatenation of s and t into the arena. 

### Free the whole arena

```void free_arena(arena the_arena);```
All objects in the arena are reclaimed.  There is no memory cleanup, so dangling pointers might not be identified immediately. 

