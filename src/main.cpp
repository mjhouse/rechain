#include "blockchain.hpp"
#include "block.hpp"
#include "keys.hpp"

int main( int argc, char** argv ){

	std::shared_ptr<Block> genesis(new Block());
	genesis->set_message("This is the genesis block");

	std::shared_ptr<Block> blockA(new Block());
	blockA->set_message("This is the first real block");

	std::unique_ptr<BlockChain> blockchain( new BlockChain( "rechain.blockchain" ) );

	KeyPair key_pair("data/rsa.public","data/rsa.private");

	key_pair.sign(genesis);
	key_pair.sign(blockA);

	blockchain->publish( genesis );
	blockchain->publish( blockA	);

	std::weak_ptr<Block> a = blockchain->get(0);
	std::weak_ptr<Block> b = blockchain->get(1);

	blockchain->print();

	return 0;
}
