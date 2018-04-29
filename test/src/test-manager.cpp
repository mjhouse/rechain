#include "catch.hpp"
#include "manager.hpp"

#include <stdlib.h>
#include <sstream>
#include <string>
#include <fstream>

extern std::string dump_file( std::string path );
extern void copy_file( std::string in, std::string out );
extern std::string get_path( std::string partial );
extern char* get_path_char( std::string partial );

SCENARIO( "manager is used without environment variables", "[.][manager]" ){

    GIVEN( "a test environment without variables" ){
   
        // manager instance to test
        Manager manager;

        // set RECHAIN_HOME
        char* home = std::getenv("RECHAIN_HOME");
        putenv((char*)"RECHAIN_HOME=");

        // configure the manager and save the result
        bool configure_result = manager.configure(Level::none);

        WHEN( "configure is called on the manager" ){

            THEN( "manager fails to configure" ){
                REQUIRE_FALSE( configure_result );    
            }

        }

        WHEN( "a file is published by the manager" ){
            std::string file = get_path("files/general/test_publish.txt");   
            bool result = manager.publish(file);

            std::string grey = dump_file(get_path("files/tmp/rechain.blockchain"));

            THEN( "manager fails to publish" ){
                REQUIRE_FALSE(result);
                REQUIRE(grey.empty());
            }
        }

    }

	GIVEN( "a test environment with variables set" ){

        // manager instance to test
        Manager manager;

		// set RECHAIN_HOME
        std::string home_path = "RECHAIN_HOME=" + get_path("files/tmp");
        putenv((char*)home_path.c_str());

		// create current keys
		copy_file(get_path("keys/rsa.public"),get_path("files/tmp/current.public"));
		copy_file(get_path("keys/rsa.private"),get_path("files/tmp/current.private"));

        // get the key files as strings
        std::string priv_gold = dump_file(get_path("keys/user1.private"));
        std::string publ_gold = dump_file(get_path("keys/user1.public"));
      
        // configure the manager and save the result
        bool configure_result = manager.configure(Level::none);

        WHEN( "configure is called on the manager" ){

            THEN( "manager runs configuration successfully" ){
                REQUIRE( configure_result );    
            }

        }

        WHEN( "a file is published by the manager" ){
            std::string file = get_path("files/general/test_publish.txt");   
            bool result = manager.publish(file);

            THEN( "manager successfully  publishes" ){
                REQUIRE(result);
            }
        }

    }
}
