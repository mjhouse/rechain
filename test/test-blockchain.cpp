#include "catch.hpp"
#include "blockchain.hpp"
#include "block.hpp"
#include "record.hpp"
#include "logger.hpp"
#include "keys.hpp"

#include <cryptopp/osrng.h>		// For AutoSeededRandomPool
#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace

#include <cereal/archives/json.hpp>

#include <string>

#define DUMP_FILE(X) (std::string( std::istreambuf_iterator<char>(X),std::istreambuf_iterator<char>()))
#define NUM_RECORDS 10
#define NUM_BLOCKS  4

extern std::string gen_random();

SCENARIO( "records are added to blockchain and mined", "[blockchain][blockchain-mine]" ){

	GIVEN( "a valid blockchain" ){

		BlockChain blockchain;
		std::vector<std::string> signatures;
		std::vector<std::string> hashes;
		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file("test/data/keys/test.private"));

		for(unsigned int j = 0; j < NUM_BLOCKS; ++j){
			for(unsigned int i = 0; i < NUM_RECORDS; ++i){
				Record record(gen_random());
				private_key->sign(record);
				signatures.push_back(record.signature());

				blockchain.add(record);
			}

			std::string hash = blockchain.mine();
			hashes.push_back(hash);
		}

		WHEN( "valid records are added to the blockchain and mined" ){
			THEN( "blockchain adds them" ){	
				REQUIRE(blockchain.size() == NUM_BLOCKS);
				int count = 0;
				for(unsigned int i = 0 ; i < blockchain.size(); ++i){
					Block& block = blockchain[i];
					for(unsigned int j = 0; j < block.size(); ++j){
						REQUIRE(blockchain[i][j].signature() == signatures[count]);
						count++;
					}

					REQUIRE(block.hash() < HASH_MAX);
					REQUIRE(block.hash() == hashes[i]);
				}
			}
		}
	}
}

SCENARIO( "blockhain is accessed for blocks or records", "[blockchain][blockchain-access]" ){

	GIVEN( "a valid blockchain" ){

		BlockChain blockchain;
		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file("test/data/keys/test.private"));
		
		std::vector<std::string> signatures;
		std::vector<std::string> hashes;


		for(unsigned int j = 0; j < NUM_BLOCKS; ++j){
			for(unsigned int i = 0; i < NUM_RECORDS; ++i){
				Record record(gen_random());
				private_key->sign(record);
				signatures.push_back(record.signature());

				blockchain.add(record);
			}

			std::string hash = blockchain.mine();
			hashes.push_back(hash);
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
			auto bad  = blockchain.find(gen_random());

			THEN( "if block exists, blockchain returns iterator point to block" ){
				Block& b = *good;
				REQUIRE_FALSE(good == blockchain.end());
				REQUIRE(b.hash() == hashes[NUM_BLOCKS/2]);
			}

			THEN( "if block doesn't exist, blockchain returns 'end()'" ){
				REQUIRE(bad == blockchain.end());
			}
		}

		WHEN( "blockchain is queried for hash" ){
		
			THEN( "returns true if hash exists" ){
				std::string hash = hashes[NUM_BLOCKS/2];
				REQUIRE(blockchain.contains(hash));
			}

			THEN( "returns false if hash doesn't exist" ){
				REQUIRE_FALSE(blockchain.contains(gen_random()));
			}
		}

		WHEN( "blockchain should be valid" ){
		
			THEN( "blockchain is valid" ){
				REQUIRE(blockchain.valid());
			}
		}

	}
}

SCENARIO( "blockchain is altered", "[blockchain][blockchain-altered]" ){

	GIVEN( "a valid blockchain" ){

		BlockChain blockchain;
		blockchain.load("test/data/files/gold/gold.blockchain");

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file("test/data/keys/test.private"));
		std::shared_ptr<PrivateKey> user1_key(PrivateKey::load_file("test/data/keys/user1.private"));

		std::vector<std::string> signatures;
		std::vector<std::string> hashes;

		for(auto b : blockchain){
			hashes.push_back(b.hash());
			for(auto r : b){
				signatures.push_back(r.signature());
			}
		}

		//Logger::get().with( Log("console",STDOUT,Level::info) );

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
				REQUIRE_FALSE(blockchain.valid());
			}
		}

		WHEN( "record is re-signed in chain" ){
			int idx = NUM_BLOCKS/2;
			Block& b = blockchain[idx];
			
			std::shared_ptr<PrivateKey> key(PrivateKey::empty());
			key->generate();
			key->sign(b[0]);	

			THEN( "blockchain is invalid because hashes are bad" ){
				REQUIRE_FALSE(blockchain.valid());
			}
		}

		WHEN( "record is altered and blocks are re-mined" ){

			// Select a block
			int idx = NUM_BLOCKS/2;
			Block& b = blockchain[idx];
			Record& r = b[0];

			// Alter a record in the block
			r.reference(gen_random());
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
