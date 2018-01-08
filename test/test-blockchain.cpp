#include "catch.hpp"

#include "blockchain.hpp"
#include "block.hpp"
#include "data.hpp"
#include "keys.hpp"

#include <iostream>

TEST_CASE( "blockchain tests", "[blockchain]" ){

	std::shared_ptr<PublicKey> public_key( PublicKey::load_file("test/data/rsa.public") );
	std::shared_ptr<PrivateKey> private_key( PrivateKey::load_file("test/data/rsa.private") );

	std::shared_ptr<PublicKey> user1_pubkey( PublicKey::load_file("test/data/user1.public") );
	std::shared_ptr<PrivateKey> user1_privkey( PrivateKey::load_file("test/data/user1.private") );

	std::shared_ptr<PublicKey> user2_pubkey( PublicKey::load_file("test/data/user2.public") );
	std::shared_ptr<PrivateKey> user2_privkey( PrivateKey::load_file("test/data/user2.private") );

	BlockChain blockchain;

	std::string b1_hash = blockchain.new_block()
		.with_data( Data( Address( "GENESIS", "", DataType::Publication), private_key.get() ) )
		.with_data( Data( Address( "REAL_PUBLICATION", "", DataType::Publication), user1_privkey.get() ) )
		.with_data( Data( Address( "REAL_PUBLICATION", "", DataType::Publication), user2_privkey.get() ) )
		.mine();

	std::vector<std::string> hashes;
	hashes.push_back( b1_hash );

	SECTION( "blockchain is can be accessed with operator[]" ){
		auto b = blockchain[0];
		REQUIRE(b.hash() == b1_hash);
	}
	SECTION( "blockchain returns correct individual trust values" ){
		blockchain.new_block()
			.with_data( Data( Address( blockchain[0][1].get_signature(), b1_hash, DataType::Signature), private_key.get() ) )
			.with_data( Data( Address( blockchain[0][2].get_signature(), b1_hash, DataType::Signature), user1_privkey.get() ) )
			.with_data( Data( Address( blockchain[0][1].get_signature(), b1_hash, DataType::Signature), user2_privkey.get() ) )
			.mine();

		std::string pub_1 = blockchain[0][1].get_signature();
		float pub1_trust = blockchain.get_publication_trust(pub_1);
	
		std::string pub_2 = blockchain[0][2].get_signature();
		float pub2_trust = blockchain.get_publication_trust(pub_2);

		float usr1_trust = blockchain.get_user_trust(user1_pubkey->to_string());
		float usr2_trust = blockchain.get_user_trust(user2_pubkey->to_string());

		REQUIRE(pub1_trust == 0.625f);
		REQUIRE(pub2_trust == 0.25f);
		REQUIRE(usr1_trust == 0.375f);
		REQUIRE(usr2_trust == 0.125f);
	}
	SECTION( "blockchain is iterable" ){
		for(auto b : blockchain){
			auto it = std::find(hashes.begin(),hashes.end(),b.hash());
			REQUIRE_FALSE(it == hashes.end());
		}
	}
	SECTION( "blockchain can be copied" ){
		BlockChain b;
		b = blockchain;
		REQUIRE(b.get_user_trust() == blockchain.get_user_trust());
		REQUIRE(b.get_publication_trust() == blockchain.get_publication_trust());
		REQUIRE(b.size() == blockchain.size());
	}
	SECTION( "blockchain can mine valid blocks" ){
		blockchain.new_block()
			.with_data( Data( Address( "REAL_PUBLICATION", "", DataType::Publication), user1_privkey.get() ) )
			.mine();
		
		REQUIRE(blockchain.size() == 2);	
	}
	SECTION( "blockchain can add signature blocks" ){
		blockchain.new_block()
			.with_data( Data( Address( blockchain[0][1].get_signature(), b1_hash, DataType::Signature), private_key.get() ) )
			.with_data( Data( Address( blockchain[0][2].get_signature(), b1_hash, DataType::Signature), user1_privkey.get() ) )
			.with_data( Data( Address( blockchain[0][1].get_signature(), b1_hash, DataType::Signature), user2_privkey.get() ) )
			.mine();
		REQUIRE(blockchain.size() == 2);
	}
	SECTION( "blockchain correctly reports user trust" ){
		blockchain.new_block()
			.with_data( Data( Address( blockchain[0][1].get_signature(), b1_hash, DataType::Signature), private_key.get() ) )
			.with_data( Data( Address( blockchain[0][2].get_signature(), b1_hash, DataType::Signature), user1_privkey.get() ) )
			.with_data( Data( Address( blockchain[0][1].get_signature(), b1_hash, DataType::Signature), user2_privkey.get() ) )
			.mine();
		
		auto trust = blockchain.get_user_trust();

		std::string k_0 = public_key->to_string();
		std::string k_1 = user1_pubkey->to_string();
		std::string k_2 = user2_pubkey->to_string();

		REQUIRE( trust[k_0] == 0.5f );
		REQUIRE( trust[k_1] == 0.375f );
		REQUIRE( trust[k_2] == 0.125f );
	}
	SECTION( "blockchain correctly reports publication trust" ){
		blockchain.new_block()
			.with_data( Data( Address( blockchain[0][1].get_signature(), b1_hash, DataType::Signature), private_key.get() ) )
			.with_data( Data( Address( blockchain[0][2].get_signature(), b1_hash, DataType::Signature), user1_privkey.get() ) )
			.with_data( Data( Address( blockchain[0][1].get_signature(), b1_hash, DataType::Signature), user2_privkey.get() ) )
			.mine();
		
		auto trust = blockchain.get_publication_trust();
		
		std::string sig1 = blockchain[0][1].get_signature();
		std::string sig2 = blockchain[0][2].get_signature();

		REQUIRE(trust[sig1] == 0.625f);
		REQUIRE(trust[sig2] == 0.25f);
	}
}
