#include "test-framework.hpp"
#include "test-publication.hpp"

#include <iostream>

namespace publication_tests {

    std::vector<failure> failures;

    test_case tests[] = {
    
        "construct a publication record",[]
        {
            
        }
    
    };

    bool run(){
        for(auto& test : tests){
            try {
                test.run();
            }
            catch(failure& f){
                std::cout << f.name << ": " << f.file << ": " << f.line << std::endl;
                failures.append(f);
            }
        }

        return false;
    }

}
