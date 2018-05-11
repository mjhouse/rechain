#include "catch.hpp"

extern std::string dump_file( std::string path );
extern void copy_file( std::string in, std::string out );
extern std::string get_path( std::string partial );
extern char* get_path_char( std::string partial );

SCENARIO( "we want to run some one-off tests", "[.transient]" ){

	GIVEN( "some things. to test. like you do." ){

        THEN( "a thing that you expected to happen does" ){
        }

    }

}
