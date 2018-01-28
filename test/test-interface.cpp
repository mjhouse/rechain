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

    GIVEN( "a test environment without variables" ){
    

        WHEN( "any operation is performed" ){
            char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--private_key",
                (char*)"test/data/keys/user1.private",
                (char*)"--public_key",
                (char*)"test/data/keys/user1.public",
                (char*)"--silent"
            };
            int argc = 6;

            // set RECHAIN_HOME
            char* home = std::getenv("RECHAIN_HOME");
            putenv((char*)"RECHAIN_HOME=");

            Interface i(argc,argv);
            int result = i.execute();

            THEN( "interface fails to run" ){
                REQUIRE_FALSE( result == 0 );    
            }
        }

    }

	GIVEN( "a test environment with variables set" ){

		// set RECHAIN_HOME
		putenv((char*)"RECHAIN_HOME=test/data/files/tmp");

		// create current keys
		move("test/data/keys/rsa.public","test/data/files/tmp/current.public");
		move("test/data/keys/rsa.private","test/data/files/tmp/current.private");

        // get the key files as strings
        std::string priv_gold = dump("test/data/files/keys/user1.private");
        std::string publ_gold = dump("test/data/files/keys/user1.public");
       
		WHEN( "interface is called with --help argument" ){

            // redirect cout to capture help message
            std::streambuf* buf = std::cout.rdbuf();
            std::ostringstream buffer;
            std::cout.rdbuf( buffer.rdbuf() );

			char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--help",
                (char*)"--silent"
            };
			int argc = 3;

            std::string help_gold = dump("test/data/files/gold/interface_help.gold");

			THEN("interface prints help message"){
				Interface i(argc,argv);
                int result = i.execute();

				REQUIRE(result == 0);
				REQUIRE(help_gold == buffer.str());

                // Set cout back to original buf
                std::cout.rdbuf( buf );
			}
		}

        WHEN( "interface is called with --version argument" ){

            // redirect cout to avoid printing version
            std::streambuf* buf = std::cout.rdbuf();
            std::ostringstream buffer;
            std::cout.rdbuf( buffer.rdbuf() );
			
            char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--version",
                (char*)"--silent"
            };
			int argc = 3;

			THEN("interface prints version information"){
				Interface interface(argc,argv);
                int result = interface.execute();

				REQUIRE(result == 0);
                REQUIRE_FALSE(buffer.str().empty());

                // Set cout back to original buf
                std::cout.rdbuf( buf );
			}
        }

        WHEN( "interface is called with bad commandline arguments" ){
			char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--BADARG",
                (char*)"--silent"
            };
			int argc = 3;

			THEN("interface fails to run"){
				Interface interface(argc,argv);
                int result = interface.execute();

				REQUIRE_FALSE(result == 0);
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
				
                Interface interface(argc,argv);
				int result = interface.execute();

                REQUIRE(result == 0);
                REQUIRE(cur_priv == priv_gold);
                REQUIRE(cur_publ == publ_gold);
			}
		}

        WHEN( "interface is used to load a malformed key pair" ){
            THEN( "bad public key fails to load" ){
                char* argv[] = {
                    (char*)"./bin/rechain",
                    (char*)"--private_key",
                    (char*)"test/data/keys/user1.private",
                    (char*)"--public_key",
                    (char*)"test/data/files/black/interface_key_public.black",
                    (char*)"--silent"
                };
                int argc = 6;
				
                Interface interface(argc,argv);
				int result = interface.execute();

                REQUIRE_FALSE(result == 0);
            }

            THEN( "bad private key fails to load" ){
                char* argv[] = {
                    (char*)"./bin/rechain",
                    (char*)"--private_key",
                    (char*)"test/data/files/black/interface_key_private.black",
                    (char*)"--public_key",
                    (char*)"test/data/keys/user1.public",
                    (char*)"--silent"
                };
                int argc = 6;
				
                Interface interface(argc,argv);
				int result = interface.execute();

                REQUIRE_FALSE(result == 0);
            }
        }

		WHEN( "interface is used to publish a valid document" ){
			THEN("document is published to blockchain"){
                char* argv[] = {
                    (char*)"./bin/rechain",
                    (char*)"--publish",
                    (char*)"test/data/files/general/test_publish.txt",
                    (char*)"--silent"
                };
                int argc = 4;
				
                Interface interface(argc,argv);
				int result = interface.execute();

                std::string gold = dump("test/data/files/gold/interface_publish.gold");
                std::string grey = dump("test/data/files/tmp/rechain.blockchain");

                // signatures are different every
                // time, so we have to strip them out
                gold.erase(1158,768);
                grey.erase(1158,768);

                REQUIRE(result == 0);
                REQUIRE(grey == gold);
			}
		}

		WHEN( "interface is used to publish a non-existing document" ){
			THEN("document is not published to blockchain"){
                char* argv[] = {
                    (char*)"./bin/rechain",
                    (char*)"--publish",
                    (char*)"test/data/files/NOEXIST/test_publish.txt",
                    (char*)"--silent"
                };
                int argc = 4;
				
                Interface interface(argc,argv);
				int result = interface.execute();

                REQUIRE_FALSE(result == 0);
			}
		}

		WHEN( "interface is used to publish a previously published document" ){
			THEN("document is not published to blockchain"){
                char* argv[] = {
                    (char*)"./bin/rechain",
                    (char*)"--publish",
                    (char*)"test/data/files/general/test_publish.txt",
                    (char*)"--silent"
                };
                int argc = 4;
			
                // overwrite the blockchain with one that already has this file published in it
                move("test/data/files/gold/interface_already_published.gold",
                     "test/data/files/tmp/rechain.blockchain");

                Interface interface(argc,argv);
				int result = interface.execute();

                REQUIRE_FALSE(result == 0);
			}
		}

		WHEN( "interface is used to check a valid blockchain" ){
            char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--check"
            };
            int argc = 2;

            move("test/data/files/gold/blockchain_general.gold",
                 "test/data/files/tmp/rechain.blockchain");

			THEN("blockchain is valid"){
				Interface interface(argc,argv);
                int result = interface.execute();
                REQUIRE(result == 0);
			}
		}

		WHEN( "interface is used to check an invalid blockchain" ){
            char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--check",
                (char*)"--silent"
            };
            int argc = 3;

            move("test/data/files/black/blockchain_duplicate_reference.black",
                 "test/data/files/tmp/rechain.blockchain");

			THEN("blockchain is invalid"){
				Interface interface(argc,argv);
                int result = interface.execute();
                REQUIRE_FALSE(result == 0);
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


	}
}
