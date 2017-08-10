# HAllocators
** This is an Afternoon Project -quick small projects that make life a bit easier- **

A bunch of straighforward and simple heap memory allocators. They allocates the needed memory upfront and reuse blocks when freed.

### RawBlocks
Can only allocate a single block at a time, and returns a void pointer to it.

### TypeBlocks
Can only allocate a single object of the type assigned to it at a time.

### VarysAllocator
Works in a way similar to RawBlocks but it can allocate multiple blocks with a single call depending on the size it is asked to allocate.

### Advantages
1. All memory is freed once allocator destructor is called; makes it safer even with raw pointers.
2. No need to make a call to the OS to allocate memory every time; slightly faster. *(benchmarking will be done later)*
3. More convenient. At least for me.

### Overriding *new* and *delete*
If you would like to use an allocator for all your memory needs, you may override the operators *new* and *delete* as follows. Currently only *VarysAllocator* could be used in this context.

``` C++
const hsize_t block_size = ...;
const hint_t num_blocks = ...;
hallocators::VarysAllocator<block_size, num_blocks> allocator;

void * operator new (std::size_t n) throw(std::bad_alloc)
{
    return allocator.alloc(n);
}

void operator delete (void * p) throw()
{
    allocator.free(p);
}
```
