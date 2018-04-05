#include "catch.hpp"
#include "interface.hpp"

#include <stdlib.h>
#include <sstream>
#include <string>
#include <fstream>

extern std::string dump_file( std::string path );
extern void copy_file( std::string in, std::string out );
extern std::string get_path( std::string partial );
extern char* get_path_char( std::string partial );

#define P(x)(std::cout << x << std::endl)

SCENARIO( "interface is called with various argc/argv values", "[interface]" ){

    GIVEN( "a test environment without variables" ){
    

        WHEN( "any operation is performed" ){
            char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--private_key",
                get_path_char("keys/user1.private"),
                (char*)"--public_key",
                get_path_char("keys/user1.public"),
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
        std::string home_path = "RECHAIN_HOME=" + get_path("files/tmp");
        putenv((char*)home_path.c_str());

		// create current keys
		copy_file(get_path("keys/rsa.public"),get_path("files/tmp/current.public"));
		copy_file(get_path("keys/rsa.private"),get_path("files/tmp/current.private"));

        // get the key files as strings
        std::string priv_gold = dump_file(get_path("keys/user1.private"));
        std::string publ_gold = dump_file(get_path("keys/user1.public"));
       
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

            std::string help_gold = dump_file(get_path("files/gold/interface_help.gold"));

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
                    get_path_char("keys/user1.private"),
                    (char*)"--public_key",
                    get_path_char("keys/user1.public"),
                    (char*)"--silent"
                };
                int argc = 6;

                Interface interface(argc,argv);
				int result = interface.execute();

                std::string cur_priv = dump_file(get_path("files/tmp/current.private"));
                std::string cur_publ = dump_file(get_path("files/tmp/current.public"));

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
                    get_path_char("keys/user1.private"),
                    (char*)"--public_key",
                    get_path_char("files/black/interface_key_public.black"),
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
                    get_path_char("files/black/interface_key_private.black"),
                    (char*)"--public_key",
                    get_path_char("keys/user1.public"),
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
                    get_path_char("files/general/test_publish.txt"),
                    (char*)"--silent"
                };
                int argc = 4;
			
                std::remove(get_path_char("files/tmp/rechain.blockchain"));

                Interface interface(argc,argv);
                int result = interface.execute();

                std::string gold = dump_file(get_path("files/gold/interface_publish.gold"));
                std::string grey = dump_file(get_path("files/tmp/rechain.blockchain"));

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
                    get_path_char("files/NOEXIST/test_publish.txt"),
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
                    get_path_char("files/general/test_publish.txt"),
                    (char*)"--silent"
                };
                int argc = 4;
			
                // overwrite the blockchain with one that already has this file published in it
                copy_file(get_path("files/gold/interface_already_published.gold"),
                     get_path("files/tmp/rechain.blockchain"));

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

            copy_file(get_path("files/gold/blockchain_general.gold"),
                 get_path("files/tmp/rechain.blockchain"));

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

            copy_file(get_path("files/black/blockchain_duplicate_reference.black"),
                 get_path("files/tmp/rechain.blockchain"));

			THEN("blockchain is invalid"){
				Interface interface(argc,argv);
                int result = interface.execute();
                REQUIRE_FALSE(result == 0);
			}
		}

		WHEN( "interface is used to mine a valid block" ){
            char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--mine",
                (char*)"--silent"
            };
            int argc = 3;

            copy_file(get_path("files/gold/interface_mine.gold"),
                 get_path("files/tmp/rechain.blockchain"));

			THEN("interface mines a block"){
				Interface interface(argc,argv);
                int result = interface.execute();
                REQUIRE(result == 0);
			}
		}

        WHEN( "interface is used to mine an invalid block" ){
            char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--mine",
                (char*)"--silent"
            };
            int argc = 3;

            copy_file(get_path("files/black/interface_mine.black"),
                 get_path("files/tmp/rechain.blockchain"));

			THEN("interface fails to mine a block"){
				Interface interface(argc,argv);
                int result = interface.execute();
                REQUIRE_FALSE(result == 0);
			}
        }

		WHEN( "interface is used to sign a published record" ){
            char* argv_s[] = {
                (char*)"./bin/rechain",
                (char*)"--sign",
                (char*)"4112D7E4A309974E5392F3AB1F86FD87EC539C74F13E5C1152836276CB0211E9",
                (char*)"--silent"
            };
            int argc_s = 4;

            char* argv_m[] = {
                (char*)"./bin/rechain",
                (char*)"--mine",
                (char*)"--silent"
            };
            int argc_m = 3;

            copy_file(get_path("files/gold/interface_pre_sign.gold"),
                 get_path("files/tmp/rechain.blockchain"));

			THEN("interface signs the provided record reference"){
				Interface interface_s(argc_s,argv_s);
                int result_s = interface_s.execute();

                REQUIRE(result_s == 0);
                
				Interface interface_m(argc_m,argv_m);
                int result_m = interface_m.execute();

                REQUIRE(result_m == 0);
			}
		}

		WHEN( "interface is used to sign a non-existing record" ){
            char* argv_s[] = {
                (char*)"./bin/rechain",
                (char*)"--sign",
                (char*)"4112D7E4A309974E5392F3AB1F86F87EC539C74F13E5C1152836276CB0211E9",
                (char*)"--silent"
            };
            int argc_s = 4;

            copy_file(get_path("files/gold/interface_pre_sign.gold"),
                 get_path("files/tmp/rechain.blockchain"));

			THEN("interface fails to sign the provided record reference"){
				Interface interface_s(argc_s,argv_s);
                int result_s = interface_s.execute();

                REQUIRE_FALSE(result_s == 0);
			}
		}

		WHEN( "interface is used to list the blockchain" ){
            // redirect cout to capture help message
            std::streambuf* buf = std::cout.rdbuf();
            std::ostringstream buffer;
            std::cout.rdbuf( buffer.rdbuf() );

			char* argv[] = {
                (char*)"./bin/rechain",
                (char*)"--list",
            };
			int argc = 2;

            std::string list_gold = dump_file(get_path("files/gold/interface_list.gold"));

            copy_file(get_path("files/gold/interface_list_blockchain.gold"),
                 get_path("files/tmp/rechain.blockchain"));

			THEN("interface prints blockchain information"){
				Interface interface(argc,argv);
                int result = interface.execute();

				REQUIRE(result == 0);
				REQUIRE(list_gold == buffer.str());

                // Set cout back to original buf
                std::cout.rdbuf( buf );
			}
		}

	}
}
