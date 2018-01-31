#include "catch.hpp"
#include "blockchain.hpp"
#include "block.hpp"
#include "record.hpp"
#include "logger.hpp"
#include "keys.hpp"

#include <string>

#define NUM_RECORDS 10
#define NUM_BLOCKS  4

extern std::string generate_hash();
extern std::string get_path( std::string partial );

SCENARIO( "blockchain is loaded or saved", "[blockchain][blockchain-serial]" ){


		BlockChain blockchain;
		std::string good_path = get_path("files/gold/blockchain_general.gold");
		std::string bad_path  = get_path("files/NOEXIST/blockchain_general.gold");

		WHEN( "blockchain is loaded from valid file path" ){
	
			blockchain.load(good_path);

			THEN( "blockchain is loaded correctly" ){
				REQUIRE(blockchain.valid());
			}
		}

		WHEN( "blockchain is loaded from invalid file path" ){
	
			THEN( "blockchain fails to load correctly" ){
				REQUIRE_FALSE(blockchain.load(bad_path));
			}
		}

		WHEN( "blockchain is saved to valid file path" ){
			std::string good_save = get_path("files/general/tmp.blockchain");

			blockchain.load(good_path);
			blockchain.save(good_save);
			std::ifstream ifs(good_save);

			THEN( "blockchain saves correctly" ){
				REQUIRE(ifs.good());
			}

			std::remove(good_save.c_str());
		}

		WHEN( "blockchain is saved to an invalid file path" ){
			THEN( "blockchain fails to save correctly" ){
				REQUIRE(blockchain.load(good_path));
				REQUIRE_FALSE(blockchain.save(bad_path));
			}
		}
}

SCENARIO( "records are added to blockchain and mined", "[blockchain][blockchain-mine]" ){

	GIVEN( "a valid blockchain" ){

		BlockChain blockchain;
		blockchain.load(get_path("files/gold/blockchain_general.gold"));

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));

		std::vector<std::string> signatures;
		std::vector<std::string> hashes;

		for(auto b : blockchain){
			hashes.push_back(b.hash());
			for(auto r : b){
				signatures.push_back(r.signature());
			}
		}

		WHEN( "valid publications are added to the blockchain and mined" ){
			size_t orig_size = blockchain.size();

			for(unsigned int i = 0; i < 10; ++i){
				Record r(generate_hash());
				private_key->sign(r);

				signatures.push_back(r.signature());
				blockchain.add(r);
			}
		
			hashes.push_back(blockchain.mine());

			THEN( "blockchain adds them" ){	
				REQUIRE(blockchain.size() == orig_size+1);
				REQUIRE(blockchain.valid());

				int count = 0;
				for(auto b : blockchain){
					for(auto r : b){
						REQUIRE(r.signature() == signatures[count]);
						count++;
					}
				}
			}
		}

		WHEN( "valid signatures are added to the blockchain and mined" ){
			size_t orig_size = blockchain.size();

			for(unsigned int i = 0; i < 3; ++i){
				Record r(blockchain[i][0].reference(),blockchain[i].hash());
				private_key->sign(r);

				signatures.push_back(r.signature());
				blockchain.add(r);
			}
		
			hashes.push_back(blockchain.mine());

			THEN( "the blockchain adds them" ){
				REQUIRE(blockchain.size() == orig_size+1);
				REQUIRE(blockchain.valid());

				int count = 0;
				for(auto b : blockchain){
					for(auto r : b){
						REQUIRE(r.signature() == signatures[count]);
						count++;
					}
				}
			}
		}
	}
}

SCENARIO( "blockhain is accessed for blocks or records", "[blockchain][blockchain-access]" ){

	GIVEN( "a valid blockchain" ){

		BlockChain blockchain;
		blockchain.load(get_path("files/gold/blockchain_general.gold"));
		
		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));
		
		std::vector<std::string> signatures;
		std::vector<std::string> references;
		std::vector<std::string> hashes;

		for(auto b : blockchain){
			hashes.push_back(b.hash());
			for(auto r : b){
				signatures.push_back(r.signature());
				references.push_back(r.reference());
			}
		}

		WHEN( "blockchain should be valid" ){
		
			THEN( "blockchain is valid" ){
				REQUIRE(blockchain.valid());
			}
		}

		WHEN( "blockchain is indexed" ){

			THEN( "blockchain returns valid references" ){
				int count = 0;
				for(unsigned int i = 0; i < blockchain.size(); ++i){
					Block& b = blockchain[i];

					for(unsigned int j = 0; j < b.size(); ++j){
						Record& a = b[j];

						REQUIRE(a.signature() == signatures[count]);
						count++;

						REQUIRE(a.valid());
						REQUIRE(b.hash() == hashes[i]);
					}
				}
			}
		}
		
		WHEN( "blockchain is copied" ){
			BlockChain tmp;
			tmp = blockchain;

			THEN( "new blockchain is the same as old" ){
				REQUIRE(tmp.size() == blockchain.size());
				for(unsigned int i = 0; i < tmp.size(); ++i){
					Block& a = blockchain[i];
					Block& b = tmp[i];
					REQUIRE(b.hash() == a.hash());
				}
			}
		}

		WHEN( "blockchain is searched for hash" ){
			auto good = blockchain.find(hashes[NUM_BLOCKS/2]);
			auto bad  = blockchain.find(generate_hash());

			THEN( "if block exists, blockchain returns iterator point to block" ){
				Block& b = *good;
				REQUIRE_FALSE(good == blockchain.end());
				REQUIRE(b.hash() == hashes[NUM_BLOCKS/2]);
			}

			THEN( "if block doesn't exist, blockchain returns 'end()'" ){
				REQUIRE(bad == blockchain.end());
			}
		}

		WHEN( "blockchain is queried to see if hash exists" ){
		
			THEN( "returns true if hash exists" ){
				std::string hash = hashes[NUM_BLOCKS/2];
				REQUIRE(blockchain.contains(hash));
			}

			THEN( "returns false if hash doesn't exist" ){
				REQUIRE_FALSE(blockchain.contains(generate_hash()));
			}
		}

		WHEN( "blockchain is queried to see if reference exists" ){
		
			THEN( "returns true if reference exists" ){
				std::string ref = references[NUM_RECORDS/2];
				REQUIRE(blockchain.contains(ref,Search::RecordType));
			}

			THEN( "returns false if reference doesn't exist" ){
				REQUIRE_FALSE(blockchain.contains(generate_hash()));
			}
		}
	}
}

SCENARIO( "blockchain is altered", "[blockchain][blockchain-altered]" ){

	GIVEN( "a valid blockchain" ){

		BlockChain blockchain;
		blockchain.load(get_path("files/gold/blockchain_general.gold"));

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));
		std::shared_ptr<PrivateKey> user1_key(PrivateKey::load_file(get_path("keys/user1.private")));

		WHEN( "blockchain loaded from gold file" ){
			THEN( "blockchain is valid" ){
				REQUIRE(blockchain.valid());
			}
		}

		WHEN( "record reference is altered in chain" ){
			int idx = NUM_BLOCKS/2;
			Block& b = blockchain[idx];

			b[0].reference("BAD REFERENCE");

			THEN( "blockchain is invalid because block hash is bad" ){
				REQUIRE_FALSE(blockchain.valid());
			}
		}

		WHEN( "record block ref is altered in chain" ){
			int idx = NUM_BLOCKS/2;
			Block& b = blockchain[idx];

			b[0].block( "BAD BLOCK" );

			THEN( "blockchain is invalid because block hash is bad" ){
				REQUIRE_FALSE(b.valid());
				REQUIRE_FALSE(blockchain.valid());
			}
		}

		WHEN( "record is re-signed in chain" ){
			Block& b = blockchain[1];
			
			std::shared_ptr<PrivateKey> key(PrivateKey::empty());
			key->generate();
			key->sign(b[0]);

			THEN( "blockchain is invalid because block hash is bad" ){
				REQUIRE_FALSE(b.valid());
				REQUIRE_FALSE(blockchain.valid());
			}
		}

		WHEN( "a duplicate record is added to the blockchain" ){
			Record r(blockchain[1][0].reference());

			std::shared_ptr<PrivateKey> key(PrivateKey::empty());
			key->generate();
			key->sign(r);
			
			blockchain.add(r);
			blockchain.mine();


			THEN( "blockchain is invalid because duplicate reference" ){
				REQUIRE_FALSE(blockchain.valid());
			}
		}

		WHEN( "record is re-signed and block is re-mined" ){
			Block& b = blockchain[1];

			std::shared_ptr<PrivateKey> key(PrivateKey::empty());
			key->generate();
			key->sign(b[0]);

			b.mine();

			THEN( "blockchain is invalid because previous hash is wrong" ){
				REQUIRE_FALSE(blockchain.valid());
			}
		}

		WHEN( "record is altered, re-signed and all blocks are re-mined" ){

			// Select a block
			int idx = 1;
			Block& b = blockchain[idx];
			Record& r = b[0];

			// Alter a record in the block
			r.reference(generate_hash());
			user1_key->sign(r);
		
			// Falsify the blockchain by re-mining each block after
			// the altered block
			for(unsigned int i = idx; i < blockchain.size(); ++i){
				std::string previous = blockchain[i-1].hash();
				Block& current = blockchain[i];

				current.previous(previous);
				current.mine();
			}

			// Check that signatures are broken
			THEN( "blockchain is invalid because signature records are bad" ){
				REQUIRE_FALSE(blockchain.valid());
			}
		}
	}
}

SCENARIO( "blockchain is accessed for trust", "[blockchain][blockchain-trust]" ){

	GIVEN( "a valid blockchain" ){
	
		BlockChain blockchain;
		std::string path = get_path("files/gold/blockchain_general.gold");

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));
		std::shared_ptr<PrivateKey> user1_key(PrivateKey::load_file(get_path("keys/user1.private")));

		std::map<std::string,float> expected = {
            {"8CEB4B9EE5ADEDDE47B31E975C1D90C73AD27B6B165A1DCD80C7C545EB65B903",7.0f},
            {"A9A8D2EE0BD117F7048FC657234E0641CD78393347E7453936FD9FB84377A570",7.0f},
            {"C155EFCE2F1429CC37DA1BDE36EE478CA2EDE8DCDDC02D8C76AE2577A6B9A146",1.75f},
            {"ED6FBF3F6C3F94D0193341BABA2BE7AAF23E2A16FE78E9152BD77CD9C83B3664",6.125f}
        };

		WHEN( "blockchain calculates publication trust on load" ){

			blockchain.load(path);

			THEN( "trust should match expected values" ){
				for(auto block : blockchain){
					for(auto record : block){
						float trust = blockchain.trust( record.reference() );
                        if(expected.count(record.reference()) == 0 ){
							REQUIRE(trust == 0.0f);
						} else {
							REQUIRE(trust == expected[record.reference()]);
						}

						
					}
				}
			}
		}
	}
}
