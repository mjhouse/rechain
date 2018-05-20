#ifndef __TEST_FRAMEWORK__
#define __TEST_FRAMEWORK__

// system includes
#include <functional>
#include <fstream>
#include <cstring>
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

        std::string name;

        std::vector<test_case> tests;
        std::vector<failure> failures; 

        test_set( std::string n, std::vector<test_case> t );

        bool run();

};

class test_framework {

    private:
        std::vector<test_set> failures;

    public:

        bool run();

        void report();

};

// ---------------------------------------
// UTILITIES
inline std::string generate_hash() {
    std::string result;
    const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (int i = 0; i < 64; ++i) {
        result.append(&(alphanum[rand() % (sizeof(alphanum) - 1)]));
    }

    return result;
}

inline std::string trim_last_slash( std::string s ){
    auto it = s.end() - 1;
    if(*it == '/') s.erase(it);
    return s;
}

inline std::string trim_first_slash( std::string s ){
    auto it = s.begin();
    if(*it == '/') s.erase(it);
    return s;
}

inline std::string get_path( std::string partial ){
    std::string path = TEST_ROOT;
    return trim_last_slash(path) + "/" + trim_first_slash(partial);
}

inline char* get_path_char( std::string partial ){
    std::string full = get_path(partial);
    
    char* cstr = new char[full.length() + 1];
    std::strcpy(cstr,full.c_str());
    return cstr;
}

inline std::string dump_file( std::string path ){
    std::string content;
    std::ifstream ifs(path);
    if(ifs.is_open()){
        content = [&ifs]{
            std::ostringstream ss{};
            ss << ifs.rdbuf();
            return ss.str();}();
    }
    return content;
}

inline void copy_file( std::string in, std::string out ){
    std::ifstream src(in, std::ios::binary);
    std::ofstream dst(out, std::ios::binary);
    dst << src.rdbuf();
}

#endif
