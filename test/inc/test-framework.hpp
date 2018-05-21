#ifndef __TEST_FRAMEWORK__
#define __TEST_FRAMEWORK__

// system includes
#include <functional>
#include <fstream>
#include <cstring>
#include <csignal>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#define RCTHROW(message)  \
    throw failure( { std::string(message), std::string(__FILE__), __LINE__ } )

#define RCREQUIRE(condition)  \
    void((condition) ? 0 :    \
    throw failure( { "RCREQUIRE(" #condition ")", __FILE__, __LINE__ } ))

struct test_case {
    const char* name;
    std::function<void()> run;
};

struct failure {
    std::string name;
    std::string file;
    int line;
};

class test_set {

    public:

        static std::vector<test_set*> all_test_sets;
        static test_case* current_test_case;

        std::string name;

        std::vector<test_case> tests;
        std::vector<failure> failures; 

        test_set( std::string n, std::vector<test_case> t );

        bool run();

};

class test_framework {

    public:

        std::vector<test_set> failures;
        static test_set* current_test_set;

        bool run();

        void report();

        void catch_signals();
};

// ------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------
void signal_handler(int signal_num);

std::string generate_hash();

std::string trim_last_slash( std::string s );

std::string trim_first_slash( std::string s );

std::string get_path( std::string partial );

char* get_path_char( std::string partial );

std::string dump_file( std::string path );

void copy_file( std::string in, std::string out );

#endif
