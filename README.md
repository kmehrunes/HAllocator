# HAllocators
**This is an Afternoon Project -quick small projects that make life a bit easier-**

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

### Compiling and Running Tests
The tests are made using Catch (https://github.com/philsquared/Catch), and you need the single header file version of it. In order to run the tests using GCC you can run the following commands.

```
Execute this only once
g++ -std=c++11 tests-main.cpp -c
```

```
Execute this every time you need to re-run the tests
g++ tests-main.o all-tests.cpp -o tests && ./tests
```

### Benchmarking Results
    malloc() cost (nanoseconds)
        Count: 1000
	    Sum: 169056
	    Min: 63
	    Max: 4276
	    Range: 4213
	    Mean: 169
    free() cost (nanoseconds)
	    Count: 1000
	    Sum: 85117
	    Min: 57
	    Max: 1572
	    Range: 1515
	    Mean: 85
    
    type blocks alloc() cost (nanoseconds)
	    Count: 1000
	    Sum: 94856
	    Min: 74
	    Max: 904
	    Range: 830
	    Mean: 94
    type blocks free() cost (nanoseconds)
	    Count: 1000
	    Sum: 75677
	    Min: 61
	    Max: 260
	    Range: 199
	    Mean: 75
    
    raw blocks alloc() cost (nanoseconds)
        Count: 1000
	    Sum: 97395
	    Min: 73
	    Max: 711
	    Range: 638
	    Mean: 97
    raw blocks free() cost (nanoseconds)
        Count: 1000
	    Sum: 75291
	    Min: 58
	    Max: 351
	    Range: 293
	    Mean: 75
 
    varys alloc() cost (nanoseconds)
        Count: 1000
	    Sum: 107433
	    Min: 81
	    Max: 1530
	    Range: 1449
	    Mean: 107
    varys free() cost (nanoseconds)
	    Count: 1000
	    Sum: 50511
	    Min: 38
	    Max: 102
	    Range: 64
	    Mean: 50
    
