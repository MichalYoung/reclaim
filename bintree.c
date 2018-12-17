//
// Created by Michal Young on 2018-12-14.
//

#include "bintree.h"
#include "refcount.h"
#include <stdio.h>
#include <assert.h>

/*
 * Example objects --- leaf and internal node
 */

// void cleanup(ref_node this) {
void cleanup(void *this_ref) {
    ref_node this = (ref_node) this_ref;
    assert(this->header.magic == MAGIC);
    if (this->tag == interior) {
        // Release children to reduce their reference counts
        link(&(this->body.interior.left), NULL);
        link(&(this->body.interior.right), NULL);
    }
}


struct rfc_descriptor node_class = {
        sizeof(struct obj_node_struct),
        cleanup
};

ref_node construct_leaf(int x) {
    ref_node obj = (ref_node) construct(&node_class);
    obj->tag = leaf;
    obj->body.leaf.v = x;
    return obj;
}

ref_node construct_interior(ref_node left, ref_node right) {
    ref_node obj = (ref_node) construct(&node_class);
    obj->tag = interior;
    obj->body.interior.left = NULL;
    link(&(obj->body.interior.left), left);
    obj->body.interior.right = NULL;
    link(&(obj->body.interior.right), right);
    return obj;
}


int main(int argc, char** argv) {
    ref_node a_leaf = construct_leaf(5);
    printf("Leaf should hold 5: %d\n", a_leaf->body.leaf.v);
    ref_node twins = construct_interior(a_leaf, a_leaf);
    a_leaf = construct_leaf(7);
    ref_node root = construct_interior(twins, a_leaf);
    printf("Clearing root\n");
    link(&root, NULL);
    printf("Clearing twins\n");
    link(&twins, NULL);
    printf("All clear, storage should be free.\n");
    return 0;
}



