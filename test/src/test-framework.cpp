#include "test-framework.hpp"

std::vector<test_set*> test_set::all_test_sets = {};

test_set::test_set( std::string n, std::vector<test_case> t ){
    name = n;
    tests = t;

    test_set::all_test_sets.push_back(this);
}

bool test_set::run() {
    for(auto& test : tests){
        try {
            test.run();
        }
        catch(failure& result){
            failures.push_back(result);
        }
        catch(const std::exception& e){
            failure f({ std::string(e.what()), std::string(__FILE__), __LINE__ });
            failures.push_back(f);
        }
    }
    return (failures.size() == 0);
}

bool test_framework::run(){
    for(auto test : test_set::all_test_sets){
        bool success = test->run();
        if(!success){
            failures.push_back(*test);
        }
    }
    return (failures.size() == 0);
}

void test_framework::report(){
    for(auto& test_set : failures){
        std::cout << test_set.name << ": FAILED" << std::endl;
        for(auto& failure : test_set.failures){
           std::cout << "   " << failure.file << ": " << std::to_string(failure.line) << ": " << failure.name << std::endl; 
        }
    }
}
