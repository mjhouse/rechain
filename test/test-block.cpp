#include "catch.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

#define DATA_LIMIT	10

TEST_CASE( "block tests", "[block]" ){
	std::shared_ptr<Block> block(new Block());
	std::shared_ptr<PrivateKey> key(PrivateKey::load_file("test/data/test.private"));
	std::vector<std::string> signatures;

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

		REQUIRE_FALSE(block->add_data(d0));
		REQUIRE_FALSE(block->add_data(d1));
		REQUIRE_FALSE(block->add_data(d2));
	}
	SECTION( "block generates a hash" ){
		std::string hash = block->hash();
		REQUIRE(!hash.empty());
	}
	SECTION( "block changes hashing variables" ){
		std::string hash1 = block->hash();
		block->change_hash();
		std::string hash2 = block->hash();
	
		REQUIRE(hash1 != hash2);
	}
}
