#include "block.hpp"
#include "keys.hpp"
#include "data.hpp"
#include "blockchain.hpp"

#include <iostream>

int main( int argc, char** argv ){

	std::shared_ptr<PublicKey> public_key( PublicKey::load_file("data/rsa.public") );
	std::shared_ptr<PrivateKey> private_key( PrivateKey::load_file("data/rsa.private") );

	std::shared_ptr<PublicKey> user1_pubkey( PublicKey::load_file("data/user1.public") );
	std::shared_ptr<PrivateKey> user1_privkey( PrivateKey::load_file("data/user1.private") );

	std::shared_ptr<PublicKey> user2_pubkey( PublicKey::load_file("data/user2.public") );
	std::shared_ptr<PrivateKey> user2_privkey( PrivateKey::load_file("data/user2.private") );


	std::shared_ptr<Data> genesis_record( new Data( Address( "GENESIS", "", DataType::Publication) ) );
	private_key->sign(genesis_record);

	std::shared_ptr<Data> rec1( new Data( Address( "REAL_PUBLICATION", "", DataType::Publication) ) );
	user1_privkey->sign(rec1);

	std::shared_ptr<Data> rec2( new Data( Address( "REAL_PUBLICATION", "", DataType::Publication) ) );
	user2_privkey->sign(rec2);


	// Create a new block and add the first three records to it
	std::shared_ptr<Block> block1(new Block());
	block1->add_data(genesis_record);
	block1->add_data(rec1);
	block1->add_data(rec2);

	// Create a blockchain and mine the genesis block
	std::shared_ptr<BlockChain> blockchain(new BlockChain());
	blockchain->mine(block1);

	// Now we create another block with signatures referencing the first block/data objects
	std::shared_ptr<Block> block2(new Block());

	std::shared_ptr<Data> sig1( new Data( Address( rec1->get_signature(), block1->hash(), DataType::Signature) ) );
	private_key->sign(sig1);

	std::shared_ptr<Data> sig2( new Data( Address( rec2->get_signature(), block1->hash(), DataType::Signature) ) );
	user1_privkey->sign(sig2);

	block2->add_data(sig1);
	block2->add_data(sig2);

	blockchain->mine(block2);

	std::cout << "Owner: " << public_key->to_string().substr(50,20) << std::endl << std::endl;
	std::cout << "User1: " << user1_pubkey->to_string().substr(50,20) << std::endl << std::endl;
	std::cout << "User2: " << user2_pubkey->to_string().substr(50,20) << std::endl << std::endl;

	std::map<std::string,float> trust = blockchain->get_trust();
	for( auto element : trust ){
		std::cout << "TRUST/KEY: " 
			  << std::to_string(element.second) 
			  << " / " 
			  << element.first.substr(50,20) << std::endl << std::endl;
	}

	return 0;
}
