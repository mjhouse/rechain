#include "catch.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

#define DATA_LIMIT	10
#define HASH_MAX  	"00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

TEST_CASE( "block tests", "[block]" ){
	std::shared_ptr<Block> block(new Block());
	std::shared_ptr<PrivateKey> key(PrivateKey::load_file("test/data/test.private"));

	for(unsigned int i = 0; i < DATA_LIMIT; ++i){
		std::shared_ptr<Data> d1(new Data(Address("SIG_REF","BLOCK_REF",DataType::Signature)));
		std::shared_ptr<Data> d2(new Data(Address("SIG_REF","",DataType::Publication)));
		key->sign(d1);
		key->sign(d2);
		block->add_data(d1);
		block->add_data(d2);
	}

	SECTION( "block adds data objects" ){

		unsigned int = (2*DATA_LIMIT);
		REQUIRE(block->size()==expected);

		std::shared_ptr<Data> d(new Data(Address("SIG_REF","",DataType::Publication)));
		key->sign(d);
		block->add_data(d);

		REQUIRE(block->size()==expected+1);
	}
	SECTION( "block generates a hash" ){
		std::string hash = block->hash();
		REQUIRE(!hash.empty());
	}
	SECTION( "block mines an appropriate hash" ){
		std::string hash = block->mine();
		REQUIRE(!hash.empty());
		REQUIRE(hash <= HASH_MAX);
	}

}
