#include <catch.hpp>

#include "../hallocators.h"

using namespace hallocators;

struct Mock
{
    char bytes[32];
};

TEST_CASE( "Raw blocks tests", "[RawBlocks]" ) {
    const unsigned num_blocks = 3;
    RawBlocks<50> allocator(num_blocks);

    REQUIRE( allocator.total_blocks() == num_blocks );
    REQUIRE( allocator.available_blocks() == num_blocks);

    SECTION( "allocation within available blocks " ) {
        void *b1 = allocator.alloc();

        REQUIRE( allocator.total_blocks() == num_blocks );
        REQUIRE( allocator.available_blocks() == num_blocks - 1);

        void *b2 = allocator.alloc();

        REQUIRE( allocator.total_blocks() == num_blocks );
        REQUIRE( allocator.available_blocks() == num_blocks - 2);
        REQUIRE( b1 != b2 );
        REQUIRE( (char *)b1 - (char *)b2 == 50 ); // the allocators uses pop_back()

        allocator.free(b1);

        REQUIRE( allocator.total_blocks() == num_blocks );
        REQUIRE( allocator.available_blocks() == num_blocks - 1);

        allocator.free(b2);

        REQUIRE( allocator.total_blocks() == num_blocks );
        REQUIRE( allocator.available_blocks() == num_blocks);
    }

    SECTION( "allocation beyond available blocks ") {
        void *b1 = allocator.alloc(), *b2 = allocator.alloc(),
             *b3 = allocator.alloc();

        try
        {
            allocator.alloc();
            REQUIRE( false );
        }
        catch (const std::bad_alloc &)
        {
            REQUIRE( true );
        }
    }
}

TEST_CASE( "Type blocks tests", "[TypeBlocks]" ) {
    const unsigned num_blocks = 3;
    TypeBlocks<Mock> allocator(num_blocks);

    REQUIRE( allocator.total_blocks() == num_blocks );
    REQUIRE( allocator.available_blocks() == num_blocks);

    SECTION( "allocation within available blocks " ) {
        Mock *b1 = allocator.alloc();

        REQUIRE( allocator.total_blocks() == num_blocks );
        REQUIRE( allocator.available_blocks() == num_blocks - 1);

        Mock *b2 = allocator.alloc();

        REQUIRE( allocator.total_blocks() == num_blocks );
        REQUIRE( allocator.available_blocks() == num_blocks - 2);
        REQUIRE( b1 != b2 );
        REQUIRE( (char *)b1 - (char *)b2 >= sizeof(Mock) );

        allocator.free(b1);

        REQUIRE( allocator.total_blocks() == num_blocks );
        REQUIRE( allocator.available_blocks() == num_blocks - 1);

        allocator.free(b2);

        REQUIRE( allocator.total_blocks() == num_blocks );
        REQUIRE( allocator.available_blocks() == num_blocks);
    }

    SECTION( "allocation beyond available blocks ") {
        allocator.alloc();
        allocator.alloc();
        allocator.alloc();

        try
        {
            allocator.alloc();
            REQUIRE( false );
        }
        catch (const std::bad_alloc &)
        {
            REQUIRE( true );
        }
    }
}

TEST_CASE( "Varys tests", "[VarysAllocator]" ) {
    const unsigned num_blocks = 3;
    VarysAllocator<64, num_blocks> allocator;

    REQUIRE( allocator.total_blocks() == num_blocks );

    SECTION( "allocation within available blocks " ) {
        void *b1 = allocator.alloc(20);
        void *b2 = allocator.alloc(64);

        REQUIRE( b1 != b2 );
        REQUIRE( (char *)b2 - (char *)b1 == 64 );

        void *b3 = allocator.alloc(64);
        REQUIRE ( b3 != b1 );
        REQUIRE ( b3 != b2 );
        REQUIRE( (char *)b3 - (char *)b2 == 64 );

        allocator.free(b1);

        void *b4 = allocator.alloc(64);
        REQUIRE( b4 == b1 );

        allocator.free(b2);
    }

    SECTION( "allocation beyond available blocks ") {
        allocator.alloc(64);
        allocator.alloc(64);
        allocator.alloc(64);

        try
        {
            allocator.alloc(64);
            REQUIRE( false );
        }
        catch (const std::bad_alloc &)
        {
            REQUIRE( true );
        }
    }
}
