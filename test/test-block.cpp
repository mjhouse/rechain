#include "catch.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

#define DATA_LIMIT	10
#define TRUST_CONST	17

TEST_CASE( "block tests", "[block]" ){
	Block block;
	std::shared_ptr<PrivateKey> key(PrivateKey::load_file("test/data/test.private"));
	std::shared_ptr<PublicKey> pub_key(PublicKey::load_file("test/data/test.public"));
	std::vector<std::string> signatures;

	std::map<std::string,float> trust;
	trust.insert( std::make_pair(pub_key->to_string(),TRUST_CONST) );
	
	for(unsigned int i = 0; i < DATA_LIMIT; ++i){
		Data d1(Address("SIG_REF","BLOCK_REF",DataType::Signature));
		Data d2(Address("SIG_REF","",DataType::Publication));
		key->sign(&d1);
		key->sign(&d2);

		signatures.push_back(d1.get_signature());
		signatures.push_back(d2.get_signature());

		block.add(d1);
		block.add(d2);
	}
	
	SECTION( "block can iterate" ){
		for( auto d : block){
			auto it = std::find(signatures.begin(),signatures.end(),d.get_signature());
			REQUIRE_FALSE(it == signatures.end());
		}
	}
	SECTION( "block adds data objects" ){
		
		// Check the expected size after initialization
		unsigned int expected = (2*DATA_LIMIT);
		REQUIRE(block.size()==expected);

		// Create a new Data object and sign it
		Data d(Address("SIG_REF","",DataType::Publication));
		key->sign(&d);

		// Add it to the block and make sure the size changes
		block.add(d);
		REQUIRE(block.size()==expected+1);
	}
	SECTION( "block doesn't add invalid data" ){
		// Invalid because it doesn't have a data reference
		Data d0(Address("","",DataType::Publication));

		// Invalid because it isn't signed
		Data d1(Address("SIG_REF","",DataType::Publication));

		// Invalid because it is a signature with no block reference
		Data d2(Address("SIG_REF","",DataType::Signature));

		key->sign(&d0);
		key->sign(&d2);


		REQUIRE_FALSE(block.add(d0));
		REQUIRE_FALSE(block.add(d1));
		REQUIRE_FALSE(block.add(d2));
		
		REQUIRE(block.size() == (2*DATA_LIMIT));
	}
	SECTION( "block generates a hash" ){
		std::string hash = block.hash();
		REQUIRE(!hash.empty());
	}
	SECTION( "block fetches data by signature" ){
		std::string sig = signatures.at(0);
		auto it = block.find( sig );
		REQUIRE(it != block.end());
		REQUIRE((*it).get_signature() == sig);
	}
	SECTION( "block can be mined" ){
		std::string hash = block.mine();
		REQUIRE(hash <= HASH_MAX);
	}
	SECTION( "block is iterable" ){
		for(auto d : block){
			auto it = std::find(signatures.begin(),signatures.end(),d.get_signature());
			REQUIRE_FALSE(it == signatures.end());
		}
	}
	SECTION( "block returns data object at index" ){
		auto data_obj = block[3];
		REQUIRE(data_obj.get_signature() == signatures[3]);
	}
	SECTION( "block gets and sets previous hash" ){
		block.previous("TEST");
		REQUIRE(block.previous() == "TEST");
	}
}
