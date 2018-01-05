#include "catch.hpp"

#include "blockchain.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

#include <iostream>

TEST_CASE( "blockchain tests", "[blockchain]" ){
	std::shared_ptr<BlockChain> blockchain(new BlockChain());
	std::shared_ptr<PrivateKey> key(PrivateKey::load_file("test/data/test.private"));

	Data* d1 = new Data( Address("DREF","BREF",DataType::Publication) );
	Data* d2 = new Data( Address("DREF","BREF",DataType::Publication) );

	key->sign(d1);
	key->sign(d2);

	std::string b1_hash = blockchain->open_block()
		->with_data(d1)
		->mine();

	std::string b2_hash = blockchain->open_block()
		->with_data(d2)
		->mine();

	SECTION( "blockchain can mine valid blocks" ){
		blockchain->open_block()->mine();
		
		REQUIRE(blockchain->size() == 3);	
	}
}
