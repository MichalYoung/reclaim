# Reference counted objects

This is a sandbox for designing an AST structure (and perhaps other
linked structures) with reference counting.  ast.h and ast.c are
intended to provide generic support for reference counted objects.
bintree.h and bintree.c are an example tree structure with
reference counting.  bintree is intended to be like an abstract
base class with concrete implementations leaf and interior.

## Desiderata

* Generic: It should be possible to define additional linked data
structures that use the same underlying reference counting
mechanisms.  In particular, it must *not* be necessary to add
or change code in refcount.h or refcount.c to incorporate
additional types.

* Patterned: We must have a straightforward pattern of
names and declarations that can be followed for creating
additional linked structures.

* Natural: Programming with reference counts should be as
close as possible to idiomatic C programming with structures
and pointers.  For example, if we would reference s.field in
idiomatic C, then we prefer that s.field is the referencing
syntax with reference counting.  If we would reference a field
as p->field, then we prefer that same syntax works with reference
counting.  Discriminated unions should still work, even if they
aren't the only way to build complex structures.

* Checked: To the extend possible, we prefer using the C type
system to provide type checking. Where we can't use static
checking, it should be possible (and not unreasonably expensive)
to provide dynamic checks.

## Compromises

It does not appear possible to completely meet all of these goals
perfectly and simultaneously.   Some compromises we have made:

* In idiomatic C, pointer assignment is simple:
`p = p2` where `p2` is a compatible pointer.  For reference counting,
we may need to update two reference counts:

   * If `p` is not null before the assignment, then the
   reference count of *p must be decremented.
   * If `p2` is not null, then the reference count of *p2
   must be incremented.  The increment of `*p2` should
   precede the decrement of `*p`, so that `p = p` is harmless.

   To manage this, as well as cleanup when a reference count
   reaches zero, pointer assignment must be handled by a
   function or macro.  We have chosen to use a function,
   `link(&p, p2)`.

* The available subtype relations in C cannot support full genericity
  and at the same time full checking.  In particular, we want the
  `link` function to be applicable to any reference counted pointer
  type, without casting, but we cannot have `refcount.h`
  include a discriminated union for
  all reference-counted structures that may be created.  We
  compromise on checking:  `link` will accept arbitrary
  pointers.  In future we should attempt to provide some
  (optional?) dynamic checking that it is receiving appropriate
  arguments.

## Name and structure conventions

### Discriminated unions

Discriminated unions are a conventional and idiomatic way of
constructing linked structures in C.  They work well as long
as the number of variants is small.