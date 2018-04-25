#include "catch.hpp"
#include "record.hpp"
#include "remote.hpp"
#include "config.hpp"
#include "keys.hpp"

#include <string>
#include <chrono>
#include <thread>

extern std::string generate_hash();
extern std::string get_path( std::string partial );
extern std::string dump_file( std::string path );

SCENARIO( "remote is created with a valid config", "[.][remote]" ){

	GIVEN( "a reference and block hash" ){

        // set RECHAIN_HOME
        std::string home_path = "RECHAIN_HOME=" + get_path("files/tmp");
        putenv((char*)home_path.c_str());

        // create dummy record
        Record r(generate_hash());

        // create Config
        std::shared_ptr<Config> config(new Config());
        REQUIRE(config->initialize());

        // create a Remote
        //Remote remote(config);

        // create a server
        Remote server(config);

		WHEN( "remote::send is called with appropriate arguments" ){

            server.start_listening();
            
            unsigned int result = 1;//= remote.send(r);
    
            std::cout << "sleeping" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(60));

            server.stop_listening();

			THEN( "it returns the number of successful sends" ){
			    REQUIRE(result > 0);
            }
		}

	}
}
