#include <cereal/archives/json.hpp>

#include "catch.hpp"
#include "block.hpp"
#include "record.hpp"
#include "keys.hpp"

#define DUMP_FILE(X) (std::string( std::istreambuf_iterator<char>(X),std::istreambuf_iterator<char>()))

extern std::string gen_random();

SCENARIO( "keys can be created from strings or files", "[keys-create]" ){

	GIVEN( "paths to public and private keys and a block" ){

		std::string public_path = "test/data/test.public";
		std::string private_path = "test/data/test.private";

		std::string public_str;
		std::string private_str;

		Block block;

		std::ifstream pub(public_path);
		std::ifstream pri(private_path);

		if(pub.is_open() && pri.is_open()){
			public_str = DUMP_FILE(pub); 
			private_str = DUMP_FILE(pri); 
		}

		WHEN( "keys are loaded from file" ){
			std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(private_path));
			std::shared_ptr<PublicKey> public_key(PublicKey::load_file(public_path));
			
			THEN( "keys are valid" ){
				REQUIRE(private_key->valid());
				REQUIRE(public_key->valid());
			}

			THEN( "hex strings equal original string" ){
				REQUIRE(private_key->to_string() == private_str);
				REQUIRE(public_key->to_string() == public_str);
			}
		}

		WHEN( "keys are loaded from a string" ){
			std::shared_ptr<PrivateKey> private_key(PrivateKey::load_string(private_str));
			std::shared_ptr<PublicKey> public_key(PublicKey::load_string(public_str));
		
			THEN( "keys are valid" ){
				REQUIRE(private_key->valid());
				REQUIRE(public_key->valid());
			}

			THEN( "hex strings equal original string" ){
				REQUIRE(private_key->to_string() == private_str);
				REQUIRE(public_key->to_string() == public_str);
			}
		}

		WHEN( "keys loaded from a string that is not a valid key" ){
			THEN( "throws a decode error" ){
				REQUIRE_THROWS(PublicKey::load_string("NOT A KEY"));
				REQUIRE_THROWS(PrivateKey::load_string("NOT A KEY"));
			}
		}

		WHEN( "keys loaded from a file that doesn't contain a key" ){
			THEN( "throws a decode error" ){
				REQUIRE_THROWS(PublicKey::load_file("test/test-keys.cpp"));
				REQUIRE_THROWS(PrivateKey::load_file("test/test-keys.cpp"));
			}
		}

		WHEN( "keys loaded from a file that doesn't exist" ){
			THEN( "throws a value error" ){
				REQUIRE_THROWS(PublicKey::load_file("FILE_DOESNT_EXIST"));
				REQUIRE_THROWS(PrivateKey::load_file("FILE_DOESNT_EXIST"));
			}
		}

		WHEN( "key is saved to a file" ){
			std::shared_ptr<PrivateKey> private_key(PrivateKey::load_string(private_str));
			std::shared_ptr<PublicKey> public_key(PublicKey::load_string(public_str));
	
			std::string tmp_priv = "test/data/tmp_key.private";
			std::string tmp_publ = "test/data/tmp_key.public";

			private_key->save(tmp_priv);
			public_key->save(tmp_publ);

			THEN( "files contain the hex keys" ){
				std::ifstream priv(tmp_priv);
				std::ifstream publ(tmp_publ);
				
				REQUIRE(priv.is_open());
				REQUIRE(publ.is_open());
			
				std::string publ_str = DUMP_FILE(publ); 
				std::string priv_str = DUMP_FILE(priv); 

				REQUIRE(priv_str == private_str);
				REQUIRE(publ_str == public_str);
			
				std::remove(tmp_priv.c_str());
				std::remove(tmp_publ.c_str());
			}
		}

	}
}

SCENARIO( "keys can generated and used to sign/verify", "[keys-work]" ){

	GIVEN( "a pair of valid keys and a valid and invalid record" ){

		std::string public_path = "test/data/test.public";
		std::string private_path = "test/data/test.private";

		Record good_record(gen_random());
		Record bad_record("");


		std::shared_ptr<PublicKey> public_key(PublicKey::load_file(public_path));
		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(private_path));

		WHEN( "a private key is used to sign a valid record" ){
			REQUIRE_FALSE(good_record.valid());

			private_key->sign(good_record);

			THEN( "the record is valid" ){
				REQUIRE(good_record.valid());
			}
		}

		WHEN( "a private key is used to sign an invalid record" ){
			REQUIRE_FALSE(bad_record.valid());
			private_key->sign(bad_record);

			THEN( "the record is signed but still not valid" ){
				REQUIRE_FALSE(bad_record.valid());
			}
		}

		WHEN( "a public key is used to verify a valid record" ){
			REQUIRE_FALSE(good_record.valid());
			private_key->sign(good_record);

			THEN( "the verification returns true" ){
				REQUIRE(public_key->verify(good_record));
				REQUIRE(good_record.valid());
			}
		
		}

		WHEN( "a public key is used to verify an invalid record" ){
			REQUIRE_FALSE(bad_record.valid());
			private_key->sign(bad_record);

			THEN( "the verification returns true but record is still invalid" ){
				REQUIRE(public_key->verify(bad_record));
				REQUIRE_FALSE(bad_record.valid());
			}
		}

		WHEN( "a private key is generated" ){
			PrivateKey* priv_key = PrivateKey::empty();
			priv_key->generate();

			THEN( "the key is valid" ){
				REQUIRE(priv_key->valid());
			}

			delete priv_key;
		}

		WHEN( "a public key is generated from a private key" ){
		
			PublicKey* publ_key = PublicKey::empty();
			publ_key->generate(private_key.get());

			THEN( "the key is valid" ){
				REQUIRE(publ_key->valid());
			}

			THEN( "the key is the same as pre-generated" ){
				REQUIRE(publ_key->to_string() == public_key->to_string());
			}

			delete publ_key;
		}
	}
}
