#include <fstream>
#include <cstring>
#include <string>
#include <sstream>

#include "test-framework.hpp"

std::string generate_hash() {
    std::string result;
    const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (int i = 0; i < 64; ++i) {
        result.append(&(alphanum[rand() % (sizeof(alphanum) - 1)]));
    }

    return result;
}

std::string trim_last_slash( std::string s ){
    auto it = s.end() - 1;
    if(*it == '/') s.erase(it);
    return s;
}

std::string trim_first_slash( std::string s ){
    auto it = s.begin();
    if(*it == '/') s.erase(it);
    return s;
}

std::string get_path( std::string partial ){
    std::string path = TEST_ROOT;
    return trim_last_slash(path) + "/" + trim_first_slash(partial);
}

char* get_path_char( std::string partial ){
    std::string full = get_path(partial);
    
    char* cstr = new char[full.length() + 1];
    std::strcpy(cstr,full.c_str());
    return cstr;
}

std::string dump_file( std::string path ){
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

void copy_file( std::string in, std::string out ){
    std::ifstream src(in, std::ios::binary);
    std::ofstream dst(out, std::ios::binary);
    dst << src.rdbuf();
}

