//
// Example refcounted object classes,
// as discriminated union.  Abstract base class
// is 'node'.  Concrete subclasses are
// 'leaf' and 'interior'.
//

#ifndef AST_BINTREE_H
#define AST_BINTREE_H

#include "refcount.h"

enum node_class { leaf, interior };

// Forward reference to define pointer fields
struct obj_node_struct;
typedef struct obj_node_struct *ref_node;

struct leaf_struct {
    int v;
};

struct interior_struct {
    char op;
    ref_node left;
    ref_node right;
};

struct obj_node_struct {
    struct rfc_header header;
    enum node_class tag;
    union  {
        struct interior_struct interior;
        struct leaf_struct leaf;
    } body;
};


// When we use the discriminated union style, functions
// use the tag field to choose appropriate behavior.
// Note that nodes will all be as large as the largest
// variant, so there is some wasted storage.


ref_node new_leaf_node(int v);
ref_node new_interior_node(char op, ref_node left, ref_node right );

// Prefix walk applying f at each node, carrying arbitrary context object along
void prefix_each(ref_node root, void (*f)(ref_node node, void *context), void *context);

#endif //AST_BINTREE_H
