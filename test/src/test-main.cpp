
#include <iostream>

#include "test-framework.hpp"

#ifndef TEST_ROOT
    static_assert(0,"TEST_ROOT isn't set!");
#endif

int main( int argc, char* argv[] ){
    test_framework tf;
    bool result = tf.run();

    if(!result){
        tf.report();
        return 0;
    } else {
        std::cout << "ALL TESTS PASSED" << std::endl;
        return 1;
    }
}
