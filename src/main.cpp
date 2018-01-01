#include "block.hpp"
#include "keys.hpp"
#include "data.hpp"
#include "blockchain.hpp"

#include <iostream>

int main( int argc, char** argv ){
	std::shared_ptr<PublicKey> public_key( PublicKey::load_file("data/rsa.public") );
	std::shared_ptr<PrivateKey> private_key( PrivateKey::load_file("data/rsa.private") );

	std::shared_ptr<Data> data0( new Data( Address( "REFERENCE", "", DataType::Publication) ) );
	private_key->sign(data0);

	std::shared_ptr<Data> data1( new Data( Address( "REFERENCE", "BLOCK HASH", DataType::Signature ) ) );
	private_key->sign(data1);

	std::shared_ptr<Data> data2( new Data( Address( "REFERENCE", "", DataType::Publication ) ) );
	private_key->sign(data2);

	std::shared_ptr<Block> block(new Block());
	block->add_data(data0);
	block->add_data(data1);
	block->add_data(data2);

	std::shared_ptr<BlockChain> blockchain(new BlockChain());
	blockchain->set_block(block);
	blockchain->start();
	blockchain->wait();

	std::cout << blockchain->blockchain.back()->hash() << std::endl;

	return 0;
}
