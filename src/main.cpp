#include "blockchain.hpp"
#include "data_block.hpp"
#include "signature_block.hpp"
#include "keys.hpp"

int main( int argc, char** argv ){

	// set up some blocks
	std::shared_ptr<DataBlock> genesis(new DataBlock());
	genesis->set_message("This is the genesis block");

	std::shared_ptr<DataBlock> blockA(new DataBlock());
	blockA->set_message("This is the first real block");

	std::shared_ptr<DataBlock> blockB(new DataBlock());
	blockB->set_message("This is the second real block");

	// create the blockchain and give it a filename to load from
	std::unique_ptr<BlockChain> blockchain( new BlockChain( "rechain.blockchain" ) );

	// create a genesis key pair and a normal user
	KeyPair key_pair("data/rsa.public","data/rsa.private");
	KeyPair user1_keys("data/user1.public","data/user1.private");

	key_pair.sign(genesis);
	user1_keys.sign(blockA);
	user1_keys.sign(blockB);

	// publish each of the blocks
	blockchain->publish( genesis );
	blockchain->publish( blockA	);
	blockchain->publish( blockB	);

	std::shared_ptr<BasicBlock> target = blockchain->get(1);

	//blockchain->save();

	return 0;
}
