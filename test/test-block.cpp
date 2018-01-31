#include <cereal/archives/json.hpp>

#include "catch.hpp"
#include "block.hpp"
#include "record.hpp"
#include "keys.hpp"

extern std::string generate_hash();
extern std::string get_path( std::string partial );

SCENARIO( "block can be mined and generate a valid hash", "[block-mining]" ){

	const int NUM_RECORDS = 10;

	GIVEN( "a mined block with records" ){

		Block block;

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));
		std::shared_ptr<PublicKey> public_key(PublicKey::load_file(get_path("/keys/test.public")));

		std::vector<std::string> sigs;
		for(unsigned int i = 0; i < NUM_RECORDS; ++i){
			Record r(generate_hash());
			private_key->sign(r);
			sigs.push_back(r.signature());
			block.add(r);
		}

		std::string hash = block.mine();
		
		WHEN( "block should be valid" ){
			THEN( "block is valid" ){
				REQUIRE(block.valid());
				REQUIRE(block.size() == NUM_RECORDS);
				for(unsigned int i = 0; i < block.size(); ++i){
					REQUIRE(block[i].signature() == sigs[i]);
				}
			}
		}
		WHEN( "block has been mined" ){
			THEN( "block hash is valid" ){
				REQUIRE(block.hash() == hash);
				REQUIRE(block.hash() <= HASH_MAX);
			}
		}
	}
}

SCENARIO( "block is copyable and accessable", "[block-access]" ){

	const int NUM_RECORDS = 10;

	GIVEN( "a block with records" ){

		Block block;

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));
		std::shared_ptr<PublicKey> public_key(PublicKey::load_file(get_path("keys/test.public")));

		std::vector<std::string> sigs;
		std::vector<std::string> refs;
		for(unsigned int i = 0; i < NUM_RECORDS; ++i){
			Record r(generate_hash());
			private_key->sign(r);
			sigs.push_back(r.signature());
			refs.push_back(r.reference());
			block.add(r);
		}

		WHEN( "block is accessed by index" ){
			THEN( "it returns references to correct records" ){
				for(unsigned int i = 0; i < block.size(); ++i){
					REQUIRE(block[i].signature() == sigs[i]);
				}
			}
		}

		WHEN( "block is assigned" ){
			Block b;
			for(unsigned int i = 0; i < (NUM_RECORDS/2); ++i){
				Record r(generate_hash());
				private_key->sign(r);
				b.add(r);
			}

			block = b;
			THEN( "block is copied successfully" ){
				REQUIRE(block.size() == b.size());
				for(unsigned int i = 0; i < block.size(); ++i){
					REQUIRE(block[i].signature() == b[i].signature());
				}
			}
		}

		WHEN( "previous is set or returned" ){
			std::string p = block.previous("TEST");
			std::string k = block.previous();

			THEN( "set values match returned values" ){
				REQUIRE(p == k);
				REQUIRE(p == "TEST");
			}
		}

		WHEN( "block is accessed with an iterator" ){

			THEN( "block returns expected references" ){
				for(Block::iterator b_it = block.begin(); b_it != block.end(); ++b_it){
					auto r = *b_it;
					auto r_it = std::find(sigs.begin(),sigs.end(),r.signature());
					REQUIRE(r_it != sigs.end());
				}
			}
		}
	
		WHEN( "block is accessed with 'find'" ){
			auto it = block.find(refs[NUM_RECORDS/2]);
			auto r = *it;

			THEN( "it returns an iterator to the correct record" ){
				REQUIRE(r.signature() == sigs[NUM_RECORDS/2]);
				REQUIRE(r.reference() == refs[NUM_RECORDS/2]);
			}
		}

		WHEN( "block is serialized/unserialized to a file" ){
			Block a = block;
			Block b;

			std::string tmp_path = get_path("files/general/tmp.block");

			std::ofstream ofs(tmp_path);
			if(ofs.is_open()){
				cereal::JSONOutputArchive archive(ofs);
				archive( block );
			}


			std::ifstream ifs(tmp_path);
			if(ifs.is_open()){
				cereal::JSONInputArchive archive(ifs);
				archive( b );
			}

			THEN( "block has previous data" ){
				REQUIRE(a.hash() == b.hash());
				REQUIRE(a.size() == b.size());
			}
			
			std::remove(tmp_path.c_str());
		}
	}
}

SCENARIO( "block can add valid and reject invalid records", "[block-adding]" ){

	const int NUM_RECORDS = 10;

	GIVEN( "a block with records" ){

		Block block;

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));
		std::shared_ptr<PublicKey> public_key(PublicKey::load_file(get_path("keys/test.public")));

		std::vector<std::string> sigs;
		for(unsigned int i = 0; i < NUM_RECORDS; ++i){
			Record r(generate_hash());
			private_key->sign(r);
			sigs.push_back(r.signature());
			block.add(r);
		}
		WHEN( "valid blocks are added" ){
			Record r(generate_hash());
			private_key->sign(r);

			THEN( "they are accepted" ){
				REQUIRE(block.add(r));
				REQUIRE(block.size() == NUM_RECORDS+1);
				REQUIRE(block[NUM_RECORDS].signature() == r.signature());
			}
		}
		
		WHEN( "invalid blocks are added" ){
			Record a("");		// invalid because no reference
			Record b(generate_hash()); // invalid because not signed

			private_key->sign(a);

			THEN( "they are rejected" ){
				REQUIRE_FALSE(block.add(a));
				REQUIRE_FALSE(block.add(b));
				REQUIRE(block.size() == NUM_RECORDS);
			}
		}

		WHEN( "block is deliberately given an invalid record" ){
			
			// invalid because no reference
			block[3] = Record("");

			THEN( "block becomes invalid" ){
				REQUIRE_FALSE(block.valid());
			}
		}

		WHEN( "block has not been mined" ){
			Block b;

			THEN( "block hash is invalid" ){
				REQUIRE_FALSE(b.valid());
			}
		}
	}
}
