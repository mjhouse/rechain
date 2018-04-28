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

// gets set by async callback
Record callback_record;

class CallbackClass {
    public:
        void test_callback( Record& record ){
           callback_record = record; 
        }
};

void test_callback( Record& record ){
   callback_record = record; 
}

SCENARIO( "remote is created with a valid config", "[.][remote]" ){

	GIVEN( "a reference and block hash" ){

        // set RECHAIN_HOME
        std::string home_path = "RECHAIN_HOME=" + get_path("files/tmp");
        putenv((char*)home_path.c_str());

        // create Config
        std::shared_ptr<Config> config(new Config());
        REQUIRE(config->initialize());

        std::map<std::string,std::string> peers = {
            { "localhost", "8080" }
        };

        config->set_peers(peers);

        // create a Remote
        Remote remote(config);

        // create a server
        Remote server(config);

		WHEN( "remote::send is called while server listens (callback class)" ){
            // create dummy record
            Record r(generate_hash());

            CallbackClass cbc;
            server.callback(std::bind(&CallbackClass::test_callback,cbc,std::placeholders::_1));

            // start listening on localhost:8080, broadcast and
            // receive the record, and stop listening.
            server.start_listening();
            remote.send(r);
            server.stop_listening();

			THEN( "callback class is successfully called with received record" ){
			    // the callback_record should be exactly the same as the
                // given record
                REQUIRE(callback_record.string() == r.string());
            }
		}

		WHEN( "remote::send is called while server listens (callback function)" ){

            // create dummy record
            Record r(generate_hash());

            server.callback(test_callback);

            // start listening on localhost:8080, broadcast and
            // receive the record, and stop listening.
            server.start_listening();
            remote.send(r);
            server.stop_listening();

			THEN( "callback function is successfully called with received record" ){
			    // the callback_record should be exactly the same as the
                // given record
                REQUIRE(callback_record.string() == r.string());
            }
		}

	}
}
