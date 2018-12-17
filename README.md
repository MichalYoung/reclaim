# refcount
Reference counted structures in ISO C  (experimental)

Work in progress, NOT working now. Enter at your own risk. 

The goal is to support reference-counted structures in a fairly 
generic fashion, not limited to a fixed set of structures, and to 
stick fairly close to C idioms.  There will be some compromises: 
At the very least, assigning to a reference-counted link field 
must be a macro or function call, not a simple assignment. 
