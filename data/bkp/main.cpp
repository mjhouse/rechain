#include <map>
#include "blockchain.hpp"
#include "block.hpp"
#include "keys.hpp"


void print_chain( std::unique_ptr<BlockChain>& blockchain ){
	for( int i = 0; i < blockchain->size(); ++i ){
		std::cout 	<< "---------"	 << std::endl
 					<< "Message:   " << blockchain->get(i)->get_message()	<< std::endl
					<< "Hash:      " << blockchain->get(i)->get_hash()		<< std::endl
					<< "Reference: " << blockchain->get(i)->get_reference()	<< std::endl
					<< "BlockType: " << blockchain->get(i)->type()			<< std::endl;
	}
}

int main( int argc, char** argv ){


	// create the blockchain and give it a filename to load from
	std::unique_ptr<BlockChain> blockchain( new BlockChain( "rechain.blockchain" ) );

	// create a genesis key pair and a normal user
	KeyPair owner_keys("data/rsa.public","data/rsa.private");
	KeyPair user1_keys("data/user1.public","data/user1.private");

	// set up some blocks
	blockchain->create_genesis_block( owner_keys );
	blockchain->create_data_block( "This is the first real block", user1_keys );
	blockchain->create_data_block( "This is the second real block", user1_keys );
	blockchain->create_data_block( "This is the third real block", owner_keys );
	blockchain->create_signature_block( blockchain->get(1), owner_keys );
	blockchain->create_signature_block( blockchain->get(3), user1_keys );

	print_chain(blockchain);

	// -------------------------------------------------------------------------
	// Calculate trust
	std::map<std::string,int> user_trust = blockchain->get_author_trust();
	for( const auto& sm_pair : user_trust ){
		std::cout << sm_pair.first << std::endl;
		std::cout << sm_pair.second << std::endl;
	}

	//blockchain->save();

	return 0;
}
