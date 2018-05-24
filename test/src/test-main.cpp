
#include <iostream>

#include "test-framework.hpp"

#ifndef TEST_ROOT
    static_assert(0,"TEST_ROOT isn't set!");
#endif

int main( int /* argc */, char** /* argv[] */ ){
    
    test_framework tf;
    
    tf.catch_signals();
    bool result = tf.run();

    tf.report();
    if(!result){
        return 0;
    } else {
        return 1;
    }
}
