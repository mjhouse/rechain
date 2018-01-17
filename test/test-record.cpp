#include "catch.hpp"
#include "block.hpp"
#include "record.hpp"
#include "keys.hpp"

#include <cryptopp/osrng.h>		// For AutoSeededRandomPool
#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace

#include <string>

#define DUMP_FILE(X) (std::string( std::istreambuf_iterator<char>(X),std::istreambuf_iterator<char>()))

extern std::string gen_random();

SCENARIO( "records created with different initial values" ){

	GIVEN( "a reference, block hash and initial trust value" ){

		std::string reference = gen_random();
		std::string block = gen_random();
		float trust = 0.34f;

		WHEN( "record is created with only a reference" ){
			Record r(reference);

			THEN( "it reports the correct reference and type" ){
				REQUIRE(r.type() == DataType::Publication);
				REQUIRE(r.reference() == reference);
				REQUIRE(r.block() == "");
			}
		}

		WHEN( "record is created with reference and block hash" ){

			Record r(reference,block);

			THEN( "it reports the correct values and type" ){
				REQUIRE(r.reference() == reference);
				REQUIRE(r.block() == block);
				REQUIRE(r.type() == DataType::Signature);
			}
		}

		WHEN( "record is created with file iostream" ){
			std::ifstream ifs("test/data/dummy.txt");
			Record r(ifs);

			CryptoPP::SHA256 hasher;
	
			std::ifstream mfs("test/data/dummy.txt");
			std::string data = DUMP_FILE(mfs);
			std::string hash;

			CryptoPP::StringSource ss(data,true,
				new CryptoPP::HashFilter(hasher,
					new CryptoPP::HexEncoder(
						new CryptoPP::StringSink(hash)))); 

			THEN( "it reports the correct values and type" ){
				REQUIRE(r.reference() == hash);
				REQUIRE(r.type() == DataType::Publication);
			}
		}

	}

}

SCENARIO( "record methods are used to set and retrieve values" ){

	GIVEN( "a publication and signature record" ){
		std::string reference = gen_random();
		std::string block = gen_random();
		float trust = 0.34f;

		Record publication(reference);
		Record signature(reference,block);
	
		WHEN( "values are retrieved" ){
			std::string r = publication.reference();
			std::string b = signature.block();
			float t = publication.trust();

			THEN( "reference matches given reference" ){
				REQUIRE(r == reference);
			}

			THEN( "block reference matches given block reference" ){
				REQUIRE(b == block);
			}

			THEN( "trust should be 0.0f" ){
				REQUIRE(t == 0.0f);
			}
		}

		WHEN( "values are set" ){
			std::string r = publication.reference("TEST");
			std::string b = publication.block("TEST");
			float t = publication.trust(trust);

			THEN( "reference matches given reference" ){
				REQUIRE(r == "TEST");
			}

			THEN( "block reference matches given block reference" ){
				REQUIRE(b == "TEST");
			}

			THEN( "trust should be 0.0f" ){
				REQUIRE(t == trust);
			}
		}
	}
}
