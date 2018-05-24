#include "test-framework.hpp"

std::vector<test_set*> test_set::all_test_sets = {};

test_case* test_set::current_test_case = nullptr;
test_set* test_framework::current_test_set = nullptr;

int test_set::test_case_count = 0;
int test_set::test_set_count = 0;

test_set::test_set( std::string n, std::vector<test_case> t ){
    name = n;
    tests = t;

    test_set::test_set_count += 1;
    test_set::test_case_count += t.size();
    test_set::all_test_sets.push_back(this);
}

bool test_set::run() {
    for(auto& test : tests){

        test_set::current_test_case = &test;

        try {
            test.run();
        }
        catch(failure& result){
            failures.push_back(result);
        }
        catch(const std::exception& e){
            std::string tname  = std::string(test_set::current_test_case->name); 
            std::string terror = std::string(e.what());
            
            std::string msg = "(\"" + tname + "\") " + terror;
            failure f({ msg, std::string(__FILE__), __LINE__ });
            failures.push_back(f);
        }
    }
    return (failures.size() == 0);
}

bool test_framework::run(){
    for(auto test : test_set::all_test_sets){

        test_framework::current_test_set = test;

        bool success = test->run();

        if(!success){
            failures.push_back(*test);
        }

    }
    return (failures.size() == 0);
}

void test_framework::report(){

    if(failures.size() > 0){
        for(auto& test_set : failures){
            std::cout << test_set.name << ": FAILED" << std::endl;
            for(auto& failure : test_set.failures){
               std::cout << "   " << failure.file << ": " << std::to_string(failure.line) << ": " << failure.name << std::endl; 
            }
        }
    }
    else {
        std::cout << "ALL TESTS PASSING: "
                  << test_set::test_case_count 
                  << " cases in "
                  << test_set::test_set_count 
                  << " test sets." 
                  << std::endl;
    }
}

void test_framework::catch_signals(){

    // interrupt signals
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGILL, signal_handler);

    // math error / segfault signals
    std::signal(SIGFPE, signal_handler);
    std::signal(SIGSEGV, signal_handler);

}

// ------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------
void signal_handler(int signal_num) {
    std::string name;

    std::string set = std::string(test_framework::current_test_set->name);
    std::string test = std::string(test_set::current_test_case->name);

    std::string message;

    switch(signal_num){
        case 2: 
            message = "SIGINT (interrupt signal):\n";
            break;
        case 8:
            message = "SIGFPE (floating point error):\n";
            break;
        case 11: 
            message = "SIGSEGV (segmentation fault):\n";
            break;
        case 15:
            message = "SIGTERM (terminate signal):\n";
            break;
        default:
            message = "UNKNOWN (" + std::to_string(signal_num) + "):\n";
            break;
    }
   
    message += "\t\"" + set + "\" -> ";
    message += "\"" + test + "\"";

    std::cout << message << std::endl;
    exit(1);
}

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
