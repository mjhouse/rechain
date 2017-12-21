#include "blockchain.hpp"
#include "block.hpp"
#include "keys.hpp"

int main( int argc, char** argv ){

	Block genesis;
	genesis.set_message("This is the genesis block");

	Block blockA;
	blockA.set_message("This is the first real block");

	std::unique_ptr<BlockChain> blockchain( new BlockChain( "rechain.blockchain" ) );

	blockchain->publish(
		genesis,
		KeyPair("data/rsa")
	);

	blockchain->publish(
		blockA,
		KeyPair("data/rsa")
	);

	Block& a = blockchain->get(0);
	Block& b = blockchain->get(1);

	blockchain->print();
	std::cout << "#0 valid: " << a.is_valid() << std::endl;
	std::cout << "#1 valid: " << b.is_valid() << std::endl;

	return 0;
}
