#include <functional>
#include <cassert>
#include <string>

#ifndef __TEST_FRAMEWORK__
#define __TEST_FRAMEWORK__

#define ASSERT(condition)  \
    void((condition) ? 0 : \
    throw failure( { "ASSERT(" #condition ")", __FILE__, __LINE__ } ))

struct test_case {
    const char* name;
    std::function<void()> run;
};

struct failure {
    const char* name;
    const char* file;
    int line;
};

std::string generate_hash();

std::string trim_last_slash( std::string s );

std::string trim_first_slash( std::string s );

std::string get_path( std::string partial );

char* get_path_char( std::string partial );

std::string dump_file( std::string path );

void copy_file( std::string in, std::string out );

#endif
