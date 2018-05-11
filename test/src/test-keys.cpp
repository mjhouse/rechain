#include "catch.hpp"
#include "record.hpp"
#include "keys.hpp"

extern std::string generate_hash();
extern std::string dump_file( std::string path );
extern std::string get_path( std::string partial );

SCENARIO( "keys can be created from strings or files", "[keys-create]" ){

	GIVEN( "paths to public and private keys" ){

		std::string public_path = get_path("keys/test.public");
		std::string private_path = get_path("keys/test.private");

		std::string public_str = dump_file(public_path);
		std::string private_str = dump_file(private_path);



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
				REQUIRE_THROWS(PublicKey::load_file(get_path("files/general/dummy.txt")));
				REQUIRE_THROWS(PrivateKey::load_file(get_path("files/general/dummy.txt")));
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
	
			std::string tmp_priv = get_path("keys/tmp_key.private");
			std::string tmp_publ = get_path("keys/tmp_key.public");

			private_key->save(tmp_priv);
			public_key->save(tmp_publ);

			THEN( "files contain the hex keys" ){
				std::string publ_str = dump_file(tmp_publ); 
				std::string priv_str = dump_file(tmp_priv); 

				REQUIRE(priv_str == private_str);
				REQUIRE(publ_str == public_str);
			
				std::remove(tmp_priv.c_str());
				std::remove(tmp_publ.c_str());
			}
		}

		WHEN( "keys are saved to a bad file location" ){
			std::shared_ptr<PrivateKey> private_key(PrivateKey::load_string(private_str));
			std::shared_ptr<PublicKey> public_key(PublicKey::load_string(public_str));
	
			THEN( "they return false" ){
				REQUIRE_FALSE(private_key->save(get_path("NOEXIST/test")));
				REQUIRE_FALSE(public_key->save(get_path("NOEXIST/test")));
			}
		}

	}
}

SCENARIO( "keys can generated and used to sign/verify", "[keys-work]" ){

	GIVEN( "a pair of valid keys and a valid and invalid record" ){

		std::string public_path = get_path("keys/test.public");
		std::string private_path = get_path("keys/test.private");

		Record good_record(generate_hash(),"");
		Record bad_record;


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
