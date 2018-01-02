#include "catch.hpp"

#include "blockchain.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

TEST_CASE( "blockchain tests", "[blockchain]" ){
	std::shared_ptr<BlockChain> blockchain(new BlockChain());

	SECTION( "blockchain can mine a valid block" ){
		std::shared_ptr<Block> block(new Block());
		std::shared_ptr<Data> d1( new Data( Address("DREF","BREF",DataType::Publication) ) );
		std::shared_ptr<Data> d2( new Data( Address("DREF","BREF",DataType::Publication) ) );

		block->add_data(d1);
		block->add_data(d2);

		std::string hash = blockchain->mine( block );
		REQUIRE(blockchain->size() == 1);	
	}
}
