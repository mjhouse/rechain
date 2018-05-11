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

SCENARIO( "manager is is used to calculate trust", "[manager][trust]" ){

	GIVEN( "a test environment with variables" ){
		
		// set RECHAIN_HOME
        std::string home_path = "RECHAIN_HOME=" + get_path("files/tmp");
        putenv((char*)home_path.c_str());

		// create current keys
		copy_file(get_path("keys/rsa.public"),get_path("files/tmp/current.public"));
		copy_file(get_path("keys/rsa.private"),get_path("files/tmp/current.private"));

		std::remove(get_path_char("files/tmp/rechain.blockchain"));
		Manager manager; // manager to test with
		bool configure_result = manager.configure(Level::none);

		// create a genesis block
		bool genesis_result = manager.publish(get_path("files/general/test_genesis.txt"));
		bool genmine_result = manager.mine();

		REQUIRE(genesis_result);
		REQUIRE(genmine_result);

		// switch to a user, rather than the owner
		manager.set_private_key(get_path("keys/user1.private"));
		manager.set_public_key(get_path("keys/user1.public"));

        // get the key files as strings for reasons
        std::string priv_gold = dump_file(get_path("keys/user1.private"));
        std::string publ_gold = dump_file(get_path("keys/user1.public"));

		WHEN( "a user signs their own published document" ){
			// test_request.txt hash
            std::string ref = "52CC7705701AFC3125F8223F414C6C84AEB508C876ED7C58E817DFC8F39052AF";
            std::string file = get_path("files/general/test_request.txt");   

			bool p_result = false;
			bool s_result = false;
			bool m_result = false;
	
			// publish the file as a user	
            p_result = manager.publish(file);
            m_result = manager.mine();

			REQUIRE(p_result);
			REQUIRE(m_result);

			// switch to the owner
			manager.set_private_key(get_path("keys/rsa.private"));
			manager.set_public_key(get_path("keys/rsa.public"));
			
			// sign the user as the owner
			s_result = manager.sign(ref);
			m_result = manager.mine();

			REQUIRE(s_result);
			REQUIRE(m_result);

			// switch to the user again
			manager.set_private_key(get_path("keys/user1.private"));
			manager.set_public_key(get_path("keys/user1.public"));

			double initial_trust = manager.get_blockchain().trust(publ_gold);

			// sign the record with the same instance and mine
			s_result = manager.sign(ref);
			m_result = manager.mine(); 

			REQUIRE(s_result);
			REQUIRE(m_result);

			double final_trust = manager.get_blockchain().trust(publ_gold);

			THEN( "they don't gain trust" ){
				REQUIRE_FALSE(final_trust > initial_trust);
			}

		}

	}

}

SCENARIO( "manager is used with and without environment variables", "[manager]" ){

    GIVEN( "a test environment without variables" ){
        // set RECHAIN_HOME
        char* home = std::getenv("RECHAIN_HOME");
        putenv((char*)"RECHAIN_HOME=");

        WHEN( "configure is called on the manager" ){
			std::remove(get_path_char("files/tmp/rechain.blockchain"));
			Manager manager; // manager to test with
			bool configure_result = manager.configure(Level::none);

            THEN( "manager fails to configure" ){
                REQUIRE_FALSE( configure_result );    
            }

        }

        WHEN( "a file is published by the manager" ){
			std::remove(get_path_char("files/tmp/rechain.blockchain"));
			Manager manager; // manager to test with
			bool configure_result = manager.configure(Level::none);

            std::string file = get_path("files/general/test_publish.txt");   
            bool result = manager.publish(file);

            std::string grey = dump_file(get_path("files/tmp/rechain.blockchain"));

            THEN( "manager fails to publish" ){
                REQUIRE_FALSE(result);
                REQUIRE(grey.empty());
            }
        }

        WHEN( "a nonexistant file is published by the manager" ){
			std::remove(get_path_char("files/tmp/rechain.blockchain"));
			Manager manager; // manager to test with
			bool configure_result = manager.configure(Level::none);

            std::string file = get_path("files/NOEXIST/nofile.txt");   
            bool result = manager.publish(file);

            THEN( "manager fails to  publish" ){
                REQUIRE_FALSE(result);
            }
        }

        WHEN( "a file is published and mined by the manager" ){
			std::remove(get_path_char("files/tmp/rechain.blockchain"));
			Manager manager; // manager to test with
			bool configure_result = manager.configure(Level::none);

            std::string file = get_path("files/general/dummy.txt");   
            bool publish_result = manager.publish(file);
            bool mine_result = manager.mine();

            THEN( "manager fails to publish" ){
                REQUIRE_FALSE(publish_result);
                REQUIRE_FALSE(mine_result);
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

        WHEN( "configure is called on the manager" ){
            std::remove(get_path_char("files/tmp/rechain.blockchain"));
            Manager manager;
            bool configure_result = manager.configure(Level::none);

            THEN( "manager runs configuration successfully" ){
                REQUIRE( configure_result );    
            }
        }

        WHEN( "a file is published by the manager" ){
            std::remove(get_path_char("files/tmp/rechain.blockchain"));
            Manager manager;
            bool configure_result = manager.configure(Level::none);

            std::string file = get_path("files/general/test_publish.txt");   
            bool result = manager.publish(file);

            // get the contents of the new blockchain and the gold file 
            std::string gold = dump_file(get_path("files/gold/interface_publish.gold"));
            std::string grey = dump_file(get_path("files/tmp/rechain.blockchain"));

            // signatures are different every
            // time, so we have to strip them out
            gold.erase(1163,768);
            grey.erase(1163,768);

            THEN( "manager successfully  publishes" ){
                REQUIRE(result);
                REQUIRE(gold == grey);
            }
        }

        WHEN( "a nonexistant file is published by the manager" ){
            std::remove(get_path_char("files/tmp/rechain.blockchain"));
            Manager manager;
            bool configure_result = manager.configure(Level::none);

            std::string file = get_path("files/NOEXIST/nofile.txt");   
            bool result = manager.publish(file);

            THEN( "manager fails to publish" ){
                REQUIRE_FALSE(result);
            }
        }

        WHEN( "a file is published and mined by the manager" ){
            std::remove(get_path_char("files/tmp/rechain.blockchain"));
            Manager manager;
            bool configure_result = manager.configure(Level::none);

            std::string file = get_path("files/general/test_mine.txt");   
            bool publish_result = manager.publish(file);
            bool mine_result = manager.mine();

            THEN( "manager successfully publishes" ){
				REQUIRE(configure_result);
                REQUIRE(publish_result);
                REQUIRE(mine_result);
            }
        }

        WHEN( "a file is published and then requested by the manager" ){
            std::remove(get_path_char("files/tmp/rechain.blockchain"));
            Manager manager;
            bool configure_result = manager.configure(Level::none);

            std::string file = get_path("files/general/test_request.txt");   
            bool publish_result = manager.publish(file);
            bool mine_result = manager.mine();

            std::string ref = "52CC7705701AFC3125F8223F414C6C84AEB508C876ED7C58E817DFC8F39052AF";
            Record* record = manager.request(ref);

            THEN( "manager will return requested file" ){
                REQUIRE(publish_result);
                REQUIRE(mine_result);
                REQUIRE_FALSE(record == nullptr);
                REQUIRE(record->get_reference() == ref);
            }
        }

        WHEN( "a previously published file is published again (no mining)" ){
            std::remove(get_path_char("files/tmp/rechain.blockchain"));
            Manager manager;
            bool configure_result = manager.configure(Level::none);

            std::string file = get_path("files/general/test_publish.txt");   
            bool first_result = manager.publish(file);

            bool second_result = manager.publish(file);

            THEN( "manager will succeed on the first, then fail on the second" ){
				REQUIRE(configure_result);
                REQUIRE(first_result);
                REQUIRE_FALSE(second_result);
            }
        }

        WHEN( "the manager is used to validate a valid blockchain" ){
            copy_file(get_path("files/gold/blockchain_general.gold"),
                      get_path("files/tmp/rechain.blockchain"));

            Manager manager;
            bool configure_result = manager.configure(Level::none);
            bool valid_result = manager.validate();

            THEN( "validate returns true" ){
                REQUIRE(configure_result);
                REQUIRE(valid_result);
            }
        }

        WHEN( "manager tries to validate bad-hash blockchain" ){
            copy_file(get_path("files/black/blockchain_bad_hash.black"),
                      get_path("files/tmp/rechain.blockchain"));

            Manager manager;
            bool configure_result = manager.configure(Level::none);
            bool valid_result = manager.validate();

            THEN( "manager will fail to validate" ){
                REQUIRE(configure_result);
                REQUIRE_FALSE(valid_result);
            }
        }

        WHEN( "manager tries to validate bad-reference blockchain" ){
            copy_file(get_path("files/black/blockchain_bad_reference.black"),
                      get_path("files/tmp/rechain.blockchain"));

            Manager manager;
            bool configure_result = manager.configure(Level::none);
            bool valid_result = manager.validate();

            THEN( "manager will fail to validate" ){
                REQUIRE(configure_result);
                REQUIRE_FALSE(valid_result);
            }
        }

        WHEN( "manager tries to validate bad-signature blockchain" ){
            copy_file(get_path("files/black/blockchain_bad_signature.black"),
                      get_path("files/tmp/rechain.blockchain"));

            Manager manager;
            bool configure_result = manager.configure(Level::none);
            bool valid_result = manager.validate();

            THEN( "manager will fail to validate" ){
                REQUIRE(configure_result);
                REQUIRE_FALSE(valid_result);
            }
        }

        WHEN( "manager tries to validate blockchain with a duplicate ref" ){
            copy_file(get_path("files/black/blockchain_duplicate_reference.black"),
                      get_path("files/tmp/rechain.blockchain"));

            Manager manager;
            bool configure_result = manager.configure(Level::none);
            bool valid_result = manager.validate();

            THEN( "manager will fail to validate" ){
                REQUIRE(configure_result);
                REQUIRE_FALSE(valid_result);
            }
        }

        WHEN( "manager is used to mine an invalid block" ){
            copy_file(get_path("files/black/interface_mine.black"),
                      get_path("files/tmp/rechain.blockchain"));

            Manager manager;
            bool configure_result = manager.configure(Level::none);
            bool mine_result = manager.mine();

            THEN( "manager fails to mine the block" ){
                REQUIRE(configure_result);
                REQUIRE_FALSE(mine_result);
            }
        }

        WHEN( "manager is used to sign a previously published record" ){
            copy_file(get_path("files/gold/interface_pre_sign.gold"),
                      get_path("files/tmp/rechain.blockchain"));

            Manager manager;
            bool configure_result = manager.configure(Level::none);
            bool sign_result = manager.sign("4112D7E4A309974E5392F3AB1F86FD87EC539C74F13E5C1152836276CB0211E9");
            bool mine_result = manager.mine();

            THEN( "manager successfully signs and mines the signature" ){
                REQUIRE(configure_result);
                REQUIRE(sign_result);
                REQUIRE(mine_result);
            }
        }

        WHEN( "manager is used to sign a non-existant record" ){
            copy_file(get_path("files/gold/interface_pre_sign.gold"),
                      get_path("files/tmp/rechain.blockchain"));

            Manager manager;
            bool configure_result = manager.configure(Level::none);
            bool sign_result = manager.sign("4112D7E8A309974E5392F3AB1F86FD87EC539C74F13E5C1152836276CB0211E9");
            bool mine_result = manager.mine();

            THEN( "manager fails to sign or mine the record" ){
                REQUIRE(configure_result);
                REQUIRE_FALSE(sign_result);
                REQUIRE_FALSE(mine_result);
            }
        }

    }
}
