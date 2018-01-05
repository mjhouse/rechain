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



	Data* genesis_record = new Data( Address( "GENESIS", "", DataType::Publication) );
	private_key->sign(genesis_record);

	Data* rec1 = new Data( Address( "REAL_PUBLICATION", "", DataType::Publication) );
	user1_privkey->sign(rec1);

	Data* rec2 = new Data( Address( "REAL_PUBLICATION", "", DataType::Publication) );
	user2_privkey->sign(rec2);


	std::shared_ptr<BlockChain> blockchain(new BlockChain());

	std::string b1_hash = blockchain->open_block()
		->with_data( genesis_record )
		->with_data( rec1 )
		->with_data( rec2 )
		->mine();

	Data* sig1 = new Data( Address( rec1->get_signature(), b1_hash, DataType::Signature) );
	private_key->sign(sig1);

	Data* sig2 = new Data( Address( rec2->get_signature(), b1_hash, DataType::Signature) );
	user1_privkey->sign(sig2);

	Data* sig3 = new Data( Address( rec1->get_signature(), b1_hash, DataType::Signature) );
	user2_privkey->sign(sig3);

	blockchain->open_block()
		->with_data(sig1)
		->with_data(sig2)
		->with_data(sig3)
		->mine();

	std::cout << "Owner: " << public_key->to_string().substr(50,20) << std::endl << std::endl;
	std::cout << "User1: " << user1_pubkey->to_string().substr(50,20) << std::endl << std::endl;
	std::cout << "User2: " << user2_pubkey->to_string().substr(50,20) << std::endl << std::endl;
	
	for(auto b : *blockchain){
		std::cout << "BLOCK: " << b->hash().substr(50,20) << std::endl;
	}


	std::map<std::string,float> trust = blockchain->get_user_trust();
	for( auto element : trust ){
		std::cout << "TRUST/KEY: " 
			  << std::to_string(element.second) 
			  << " / " 
			  << element.first.substr(50,20) << std::endl << std::endl;
	}

	return 0;
}
