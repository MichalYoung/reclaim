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

/* Dumb tree-printer; will repeat content when the tree is
 * really a dag.  Will recurse forever on cyclic structures.
 */
void dump_tree(ref_node root) {
    if (root == NULL)  {
        printf("NULL");
        return;
    }
    printf("Node number %d  (%d references) ", root->header.obj_num, root->header.refcount);
    if (root->tag == leaf) {
        printf(" Leaf value %d\n", root->body.leaf.v);
    } else {
        printf("Interior left->%d, right-> %d\n",
                root->body.interior.left->header.obj_num,
                root->body.interior.right->header.obj_num);
        dump_tree(root->body.interior.left);
        dump_tree(root->body.interior.right);
    }
}

void test_refcount() {
    printf("Refcount test\n");
    printf("Constructing leaf\n");
    // ref_node a_leaf = construct_leaf(5);   // NOT PERMITTED!
    ref_node a_leaf; link(&a_leaf, construct_leaf(5));
    printf("Constructed leaf; "); rfc_stats();
    printf("Expected count 1\n");
    printf("Leaf should hold 5: %d\n", a_leaf->body.leaf.v);
    dump_tree(a_leaf);
    printf("\n");

    printf("\nConstructing twins\n");
    // ref_node twins = construct_interior(a_leaf, a_leaf); // NO NO NO!
    ref_node twins; link(&twins, construct_interior(a_leaf, a_leaf));
    printf("Constructed twins; "); rfc_stats();
    printf("Expected count 2\n");
    dump_tree(twins);
    printf("\n");

    printf("\nConstructing new leaf\n");
    link(&a_leaf, construct_leaf(7));
    printf("Constructed new leaf; "); rfc_stats();
    printf("Expected count 3\n");

    printf("\nConstructing new interior node from twins, a_leaf\n");
    // ref_node root = construct_interior(twins, a_leaf); // NOPE!
    ref_node root;  link(&root, construct_interior(twins, a_leaf));
    printf("Constructed interior node; "); rfc_stats();
    printf("Expected count 4\n");
    dump_tree(root);
    printf("\n");


    printf("Clearing a_leaf");
    link(&a_leaf, NULL);
    printf("Clearing root\n");
    link(&root, NULL);
    rfc_stats();
    printf("Clearing twins\n");
    link(&twins, NULL);
    printf("All clear, storage should be free.\n");
    rfc_stats();

}


int main(int argc, char** argv) {

    return 0;
}



