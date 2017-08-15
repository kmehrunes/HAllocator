#ifndef HALLOCATORS_H
#define HALLOCATORS_H

#include <vector>
#include <exception>

#include <malloc.h>
#include <math.h>
#include <stdint.h>

namespace hallocators {

typedef size_t hsize_t;
typedef unsigned short hint_t;

template<typename T>
class TypeBlocks
{
public:
    TypeBlocks(hint_t n_blocks):
        num_blocks(n_blocks), available(n_blocks)
    {
        start = new T[n_blocks];
        blocks.reserve(num_blocks);
        for (hint_t i = 0; i < num_blocks; i++)
            blocks.push_back(&start[i]);
    }

    ~TypeBlocks()
    {
        delete [] start;
    }

    T *alloc()
    {
        if (available)
        {
            T *ptr = blocks.back();
            blocks.pop_back();
            --available;
            return ptr;
        }
        throw std::bad_alloc();
    }

    void free(T *ptr)
    {
        blocks.push_back(ptr);
        ++available;
    }

    hint_t total_blocks() const
    {
        return num_blocks;
    }

    hint_t available_blocks() const
    {
        return available;
    }

private:
    T *start;
    std::vector<T *> blocks;
    hint_t num_blocks;
    hint_t available;
};

template<hsize_t SIZE>
class RawBlocks
{
public:
    RawBlocks(hint_t n_blocks):
        num_blocks(n_blocks), available(n_blocks)
    {
        start = malloc(SIZE * n_blocks);
        blocks.reserve(n_blocks);
        for (uint32_t i = 0; i < n_blocks; i++)
            blocks.push_back((char *)start + (SIZE * i));
    }

    ~RawBlocks()
    {
        free(start);
    }

    void *alloc()
    {
        if (available)
        {
            void *ptr = blocks.back();
            blocks.pop_back();
            --available;
            return ptr;
        }
        throw std::bad_alloc();
    }

    void free(void *ptr)
    {
        blocks.push_back(ptr);
        ++available;
    }

    hint_t total_blocks() const
    {
        return num_blocks;
    }

    hint_t available_blocks() const
    {
        return available;
    }

private:
    void *start;
    std::vector<void *> blocks;
    hint_t num_blocks;
    hint_t available;
};

template<hsize_t SIZE, hint_t NBLOCKS>
class VarysAllocator
{
public:
    VarysAllocator()
    {
        start = malloc(SIZE * NBLOCKS);
        for (hint_t i = 0; i < NBLOCKS; i++)
        {
            blocks[i] = (char *)start + (SIZE * i);
            lens[i] = 0;
        }
    }

    ~VarysAllocator()
    {
        free(start);
    }

    void *alloc(hsize_t size)
    {
        hint_t n = (hint_t)ceil((float)size / SIZE);
        for (hint_t i = 0; i < NBLOCKS; ++i)
        {
            if (can_satisfy(i, n))
            {
                lens[i] = n;
                return blocks[i];
            }
        }
        throw std::bad_alloc();
    }

    void free(void *ptr)
    {
        int index = ((char *)ptr - (char *)start)/SIZE;
        lens[index] = 0;
    }

    hint_t total_blocks() const
    {
        return NBLOCKS;
    }

private:
    bool can_satisfy(hint_t index, hint_t n)
    {
        hint_t max = index + n;
        for (; index < max; ++index)
        {
            if (lens[index] != 0)
                return false;
        }
        return true;
    }

private:
    void *start;
    void *blocks[NBLOCKS];
    hsize_t lens[NBLOCKS];
};

template<typename HAllocator>
class HMemory
{
public:
    template<typename T>
    T *alloc()
    {
        return static_cast<T *>(allocator.alloc(sizeof(T)));
    }

    template<typename T>
    void free(T *ptr)
    {
        allocator.free(ptr);
    }

private:
    HAllocator allocator;
};

} // namespace hallocators
#endif // HALLOCATORS_H
