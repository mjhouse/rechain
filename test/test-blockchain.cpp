#include "catch.hpp"
#include "blockchain.hpp"
#include "block.hpp"
#include "record.hpp"
#include "keys.hpp"

#include <cryptopp/osrng.h>		// For AutoSeededRandomPool
#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace

#include <cereal/archives/json.hpp>

#include <string>

#define DUMP_FILE(X) (std::string( std::istreambuf_iterator<char>(X),std::istreambuf_iterator<char>()))
#define NUM_RECORDS 10

extern std::string gen_random();

SCENARIO( "records are added to blockchain", "[blockchain access]" ){

	GIVEN( "a valid blockchain" ){

		BlockChain& blockchain = BlockChain::get_blockchain();
		std::vector<std::string> signatures;
		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file("test/data/test.private"));

		WHEN( "valid records are added to the blockchain" ){
			for(unsigned int i = 0; i < NUM_RECORDS; ++i){
				Record record(gen_random());
				private_key->sign(record);
				signatures.push_back(record.signature());

				blockchain.add(record);
			}

			blockchain.mine();

			THEN( "blockchain adds them" ){
				REQUIRE(blockchain[0].size() == NUM_RECORDS);
				for(unsigned int i = 0 ; i < blockchain[0].size(); ++i){
					REQUIRE(blockchain[0][i].signature() == signatures[i]);
				}
			}
		}
	}
}
