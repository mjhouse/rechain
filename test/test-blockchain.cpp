#include "catch.hpp"

#include "blockchain.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

#include <iostream>

TEST_CASE( "blockchain tests", "[blockchain]" ){
	std::shared_ptr<BlockChain> blockchain(new BlockChain());
	std::shared_ptr<PrivateKey> key(PrivateKey::load_file("test/data/test.private"));

	std::shared_ptr<Block> block1(new Block());
	std::shared_ptr<Block> block(new Block());

	std::shared_ptr<Data> d1( new Data( Address("DREF","BREF",DataType::Publication) ) );
	std::shared_ptr<Data> d2( new Data( Address("DREF","BREF",DataType::Publication) ) );

	key->sign(d1);
	key->sign(d2);

	block->add_data(d1);
	block1->add_data(d2);

	blockchain->mine(block);
	blockchain->mine(block1);

	SECTION( "blockchain can mine valid blocks" ){
		std::shared_ptr<Block> block3(new Block());
		blockchain->mine( block3 );
		
		REQUIRE(blockchain->size() == 3);	
	}
	SECTION( "blockchain can return a block by hash" ){
		auto b = blockchain->get_block(block1->hash());
		
		REQUIRE(b);
		REQUIRE(block1->hash() == b->hash());	
	}
	SECTION( "blockchain can erase a mined block" ){
		blockchain->discard( block1->hash() );
		auto b = blockchain->get_block( block1->hash() );

		REQUIRE_FALSE(b);
		REQUIRE( blockchain->size() == 1 );
	}
}
