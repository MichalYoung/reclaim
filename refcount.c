//
// Created by Michal Young on 2018-12-14.
//

#include "refcount.h"
#include <assert.h>
#include <stdio.h>  // Only for debugging

static int obj_count = 0;
static int obj_num = 0;

ob_ref construct(struct rfc_descriptor *desc) {
    ++obj_count;
    struct rfc_header* obj = (struct rfc_header*) malloc(desc->obj_size);
    obj->refcount = 0;
    obj->magic = MAGIC;
    obj->desc = desc;
    obj->obj_num = obj_num++;
    return obj;
}

void no_cleanup_necessary(ob_ref obj) {
    assert(obj->magic == MAGIC);
    return;
}

/*
 * Manage links.  link(&from, to) creates the
 * link and manages reference counts.
 */
//void link(ob_ref *from, ob_ref to) {
void link(void *from, void *to) {
    printf("Link!\n");
    ob_ref *from_ptr = (ob_ref *) from;
    ob_ref to_ptr = (ob_ref) to;
    assert(to_ptr == NULL || to_ptr->magic == MAGIC);
    assert(*from_ptr == NULL || (*from_ptr)->magic == MAGIC);
    if (to_ptr != NULL) to_ptr->refcount++;
    if (*from_ptr != NULL) {
        assert((*from_ptr)->refcount >= 1);  /* Fails if roots not set properly */
        printf("Reducing a reference count.\n");
        if (--((*from_ptr)->refcount) == 0) {
            printf("Invoking destructor.\n");
            (*from_ptr)->desc->destruct(*from_ptr);
            printf("Invoking free.\n");
            --obj_count;
            free(*from_ptr);
        }
    }
    *from_ptr = to_ptr;
}

void rfc_stats() {
    printf("\n*Objects in use: %d\n", obj_count);
}

