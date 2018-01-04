#include "catch.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

#define DATA_LIMIT	10
#define TRUST_CONST	17

TEST_CASE( "block tests", "[block]" ){
	std::shared_ptr<Block> block(new Block());
	std::shared_ptr<PrivateKey> key(PrivateKey::load_file("test/data/test.private"));
	std::shared_ptr<PublicKey> pub_key(PublicKey::load_file("test/data/test.public"));
	std::vector<std::string> signatures;

	std::map<std::string,float> trust;
	trust.insert( std::make_pair(pub_key->to_string(),TRUST_CONST) );
	
	for(unsigned int i = 0; i < DATA_LIMIT; ++i){
		std::shared_ptr<Data> d1(new Data(Address("SIG_REF","BLOCK_REF",DataType::Signature)));
		std::shared_ptr<Data> d2(new Data(Address("SIG_REF","",DataType::Publication)));
		key->sign(d1);
		key->sign(d2);

		signatures.push_back(d1->get_signature());
		signatures.push_back(d2->get_signature());

		block->add_data(d1);
		block->add_data(d2);
	}


	SECTION( "block removes data objects" ){
		// Remove each Data object by the save hash	
		for( std::string s : signatures ){
			block->remove_data( s );		
		}

		// Check the block is empty
		REQUIRE(block->size() == 0);
	}
	SECTION( "block adds data objects" ){
		
		// Check the expected size after initialization
		unsigned int expected = (2*DATA_LIMIT);
		REQUIRE(block->size()==expected);

		// Create a new Data object and sign it
		std::shared_ptr<Data> d(new Data(Address("SIG_REF","",DataType::Publication)));
		key->sign(d);

		// Add it to the block and make sure the size changes
		block->add_data(d);
		REQUIRE(block->size()==expected+1);
	}
	SECTION( "block doesn't add invalid data" ){
		// Invalid because it doesn't have a data reference
		std::shared_ptr<Data> d0(new Data(Address("","",DataType::Publication)));

		// Invalid because it isn't signed
		std::shared_ptr<Data> d1(new Data(Address("SIG_REF","",DataType::Publication)));

		// Invalid because it is a signature with no block reference
		std::shared_ptr<Data> d2(new Data(Address("SIG_REF","",DataType::Signature)));

		key->sign(d0);
		key->sign(d2);

		REQUIRE(block->size() == (2*DATA_LIMIT));

		REQUIRE_FALSE(block->add_data(d0));
		REQUIRE_FALSE(block->add_data(d1));
		REQUIRE_FALSE(block->add_data(d2));
		
	}
	SECTION( "block generates a hash" ){
		std::string hash = block->hash();
		REQUIRE(!hash.empty());
	}
	SECTION( "block fetches data by signature" ){
		std::string sig = signatures.at(0);
		std::shared_ptr<Data> d = block->get_data( sig );
		REQUIRE(d);
		REQUIRE(d->get_signature() == sig);
	}
	SECTION( "block changes hashing variables" ){
		std::string hash1 = block->hash();
		block->change_hash();
		std::string hash2 = block->hash();
	
		REQUIRE(hash1 != hash2);
	}
	SECTION( "block updates trust from trust map" ){
		block->set_trust(trust);
		std::vector<std::shared_ptr<Data>> data_obj = block->get_data();
		for(auto d : data_obj){
			if(d->get_data_type() == DataType::Signature){
				REQUIRE(d->get_trust() == TRUST_CONST);
			}
		}
	}
	SECTION( "block returns all data objects in vector" ){
		std::vector<std::shared_ptr<Data>> data_obj = block->get_data();
		for(auto d : data_obj){
			auto it = std::find(signatures.begin(),signatures.end(),d->get_signature());
			REQUIRE_FALSE(it == signatures.end());
		}
	}
	SECTION( "block updates trust to 0 if not in trust map" ){
		std::map<std::string,float> bad_trust;
		block->set_trust(bad_trust);
		for(auto d : block->get_data()){
			REQUIRE(d->get_trust() == 0);
		}
	}
	SECTION( "block returns data object at index" ){
		auto data_obj = block->get_data(3);
		REQUIRE(data_obj);
		REQUIRE(data_obj->get_signature() == signatures[3]);
	}
	SECTION( "block gets and sets previous hash" ){
		block->set_previous("TEST");
		REQUIRE(block->get_previous() == "TEST");
	}
}
