#include "catch.hpp"
#include "interface.hpp"

#include <stdlib.h>
#include <sstream>
#include <string>
#include <fstream>

inline std::string dump( std::string path ){
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

inline void move( std::string in, std::string out ){
    std::ifstream src(in, std::ios::binary);
    std::ofstream dst(out, std::ios::binary);
    dst << src.rdbuf();
}

SCENARIO( "use interface", "[interface]" ){

	GIVEN( "a test environment with variables set" ){

		// set RECHAIN_HOME
		putenv((char*)"RECHAIN_HOME=test/data/files/tmp");

		// create current keys
		move("test/data/keys/rsa.public","test/data/files/tmp/current.public");
		move("test/data/keys/rsa.private","test/data/files/tmp/current.private");

        // get the help output gold file
        std::string help_gold = dump("test/data/files/gold/gold.help");
        std::string priv_gold = dump("test/data/files/keys/user1.private");
        std::string publ_gold = dump("test/data/files/keys/user1.public");

        std::streambuf* buf = std::cout.rdbuf();
        std::ostringstream buffer;
        std::cout.rdbuf( buffer.rdbuf() );
       
		WHEN( "interface is used for help message" ){
			char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--help",
                (char*)"--silent"
            };
			int argc = 3;

			THEN("interface prints help message"){
				Interface i(argc,argv);
                int result = i.execute();

				REQUIRE(result == 0);
				REQUIRE(help_gold == buffer.str());
			}
		}

		WHEN( "interface is used to load a new key pair" ){
			THEN("interface copies keys to rechain home"){
                char* argv[] = {
                    (char*)"./bin/rechain",
                    (char*)"--private_key",
                    (char*)"test/data/keys/user1.private",
                    (char*)"--public_key",
                    (char*)"test/data/keys/user1.public",
                    (char*)"--silent"
                };
                int argc = 6;

                std::string cur_priv = dump("test/data/tmp/current.private");
                std::string cur_publ = dump("test/data/tmp/current.public");
				
                std::cout.rdbuf( buf );
                
                Interface i(argc,argv);
				int result = i.execute();

                REQUIRE(result == 0);
                REQUIRE(cur_priv == priv_gold);
                REQUIRE(cur_publ == publ_gold);
			}
		}

		WHEN( "interface is used to publish a document" ){
			THEN(""){
				//Interface i(argc,argv);
				//REQUIRE_FALSE(interface.execute() == 0);
			}
		}

		WHEN( "interface is used to check the blockchain" ){
			THEN(""){
				//Interface i(argc,argv);
				//REQUIRE_FALSE(interface.execute() == 0);
			}
		}

		WHEN( "interface is used to mine a blockchain" ){
			THEN(""){
				//Interface i(argc,argv);
				//REQUIRE_FALSE(interface.execute() == 0);
			}
		}

		WHEN( "interface is used to sign a published record" ){
			THEN(""){
				//Interface i(argc,argv);
				//REQUIRE_FALSE(interface.execute() == 0);
			}
		}

		WHEN( "interface is used to display the blockchain" ){
			THEN(""){
				//Interface i(argc,argv);
				//REQUIRE_FALSE(interface.execute() == 0);
			}
		}

		WHEN( "interface is used to display the version" ){
			THEN(""){
				//Interface i(argc,argv);
				//REQUIRE_FALSE(interface.execute() == 0);
			}
		}

        // Set cout back to original buf
        std::cout.rdbuf( buf );

	}
}