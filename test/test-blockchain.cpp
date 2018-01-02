#include "catch.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

TEST_CASE( "blockchain tests", "[blockchain]" ){
	std::shared_ptr<BlockChain> blockchain(new BlockChain());

	SECTION( "blockchain can mine a valid block" ){
		std::shared_ptr<Block> block(new Block());
		std::string hash = blockchain->mine( block );
		
	}
}
