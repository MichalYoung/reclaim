//
// Reference counted structures.
// Each class of managed objects has a descriptor
// that includes the object size, a constructor,
// and a destructor. The destructor knows which fields
// are pointers to other refcounted objects.
// Refcounted objects contain a header with the
// reference count and a pointer to the descriptor.
//
// Namespace management:  We use the prefix
// rfc_ to distinguish names in this module.
//

#ifndef AST_REFCOUNT_H
#define AST_REFCOUNT_H

#include <stdlib.h>


#define MAGIC (7392)  // Arbitrary value used for sanity check
/* TODO:  magic should have two fields, one to
 * identify ref-counted objects in general, and another
 * to identify a specific class of refcounted objects.
 * We can define macros to check and set the fields in an int.
 */


/*
 * Structures common to all ref-counted objects.
 * rfc_descriptor is the first part of any descriptor, but
 * we could extend it with a per-class vtable.
 * rfc_obj is the first part of any ref-counted object.
 */
struct rfc_header;
typedef struct rfc_header* ob_ref;


/* Each node type needs a destructor.  It should clear pointer
 * fields (link to NULL if reference counted, free if not
 * it is not reference counted and this is the unique owner).
 * Do NOT free storage in the destructor -- that will be
 * done after the destructor returns.
 */
typedef void (*destructor) (void *);


/*
 * rfc_descriptor is the common part before class-specific
 * functions.  obj_size is the size of the object to allocate
 * in the constructor.  Constructor should return an object of
 * that size, initializing reference count to zero.  If links
 * are initialized, it should be with reference counting,
 * not by directly assigning pointers to the link fields.
 */
struct rfc_descriptor {
    int obj_size;
    destructor destruct;
};

/* One constructor initializes any object, setting correct
 * size.  Type-specific constructors can call it and then
 * fill in the variable part.
 */
ob_ref construct(struct rfc_descriptor* desc);

/*
 * If an object contains neither reference counted links
 * nor other heap storage to be deallocated, it can use
 * the no_cleanup_necessary destructor.  Otherwise, a
 * type-specific destructor should be defined.
 */
void no_cleanup_necessary(struct rfc_header* obj);

struct rfc_header {
    int refcount;
    int magic; // Dynamic type tag to avoid booboos
    int obj_num; // Debugging aid --- each object gets a unique number
    struct rfc_descriptor* desc;
};


/*
 * Manage links.  link(&from, to) creates the
 * link and manages reference counts.
 */
// void link(ob_ref *from, ob_ref to);
void link(void *from, void *to); // So that we don't have to cast in user code

/* Print stats */
void rfc_stats();

#endif //AST_REFCOUNT_H
