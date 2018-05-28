
#include <iostream>
#include <memory>

#include "test-framework.hpp"

#include "publication_record.hpp"
#include "signature_record.hpp"
#include "genesis_record.hpp"

#include "logger.hpp"

#ifndef TEST_ROOT
    static_assert(0,"TEST_ROOT isn't set!");
#endif

int main( int /* argc */, char** /* argv[] */ ){

    // configure logger
    Logger::get()
        .with( Log("console",STDOUT,Level::debug) )
        .with( Log("log",get_path("home/test-rechain.log"),Level::debug) );

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
