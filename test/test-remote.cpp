#include "catch.hpp"
#include "record.hpp"
#include "remote.hpp"
#include "config.hpp"
#include "keys.hpp"

#include <string>

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
        Remote remote;
        REQUIRE(remote.initialize(config));

        Remote server;
        REQUIRE(server.initialize(config));

		WHEN( "remote::send is called with appropriate arguments" ){

            unsigned int result = remote.send(r);

			THEN( "it returns the number of successful sends" ){
			    REQUIRE(result > 0);
            }
		}

	}
}
