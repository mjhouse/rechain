#include "test-publication.hpp"

#ifndef TEST_ROOT
    static_assert(0,"TEST_ROOT isn't set!");
#endif

int main( int argc, char* argv[] ){
    publication_tests::run();  
}
