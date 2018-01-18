#include "catch.hpp"
#include "block.hpp"
#include "record.hpp"
#include "keys.hpp"

#include <cryptopp/osrng.h>		// For AutoSeededRandomPool
#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace

#include <cereal/archives/json.hpp>

#include <string>

#define DUMP_FILE(X) (std::string( std::istreambuf_iterator<char>(X),std::istreambuf_iterator<char>()))

extern std::string gen_random();

SCENARIO( "records created with different initial values", "[record-create]" ){

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

SCENARIO( "record methods are used to set and retrieve values", "[record-access]" ){

	GIVEN( "a publication and signature record" ){
		std::string reference = gen_random();
		std::string block = gen_random();
		float trust = 0.34f;

		Record publication(reference);
		Record signature(reference,block);
	

		WHEN( "records are signed" ){
			std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file("test/data/test.private"));
			private_key->sign(publication);

			THEN( "signed records are valid, unsigned are not" ){
				REQUIRE(publication.valid());
				REQUIRE_FALSE(signature.valid());
			}
		}

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

			THEN( "types match expected values" ){
				REQUIRE(publication.type() == DataType::Publication);
				REQUIRE(signature.type() == DataType::Signature);
			}
		}

		WHEN( "values are set" ){
			std::string r = publication.reference("TEST");
			std::string b = signature.block("TEST");
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

			THEN( "types match expected values" ){
				REQUIRE(publication.type() == DataType::Publication);
				REQUIRE(signature.type() == DataType::Signature);
			}
		}

	}
}

SCENARIO( "records are serialized or converted to strings", "[record-transform]" ){

	GIVEN( "a valid record" ){
		
		std::string reference = gen_random();
		std::string block = gen_random();
		float trust = 0.34f;

		Record record(reference,block);
		record.trust(trust);

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file("test/data/test.private"));
		private_key->sign(record);

		WHEN( "record is converted to a string" ){
			std::string automatic = record.string(true);
			std::string manual;

			manual.append(record.reference());
			manual.append(record.block());
			manual.append(record.public_key());
			manual.append(record.signature());
			manual.append(std::to_string(record.trust()));

			THEN( "string is equal to expected string" ){
				REQUIRE(manual == automatic);
			}
		}

		WHEN( "record is serialized to a file" ){
			Record empty;

			std::ofstream ofs("test/data/tmp.record");
			if(ofs.is_open()){
				cereal::JSONOutputArchive archive(ofs);
				archive( record );
			}


			std::ifstream ifs("test/data/tmp.record");
			if(ifs.is_open()){
				cereal::JSONInputArchive archive(ifs);
				archive( empty );
			}

			std::remove("test/data/tmp.record");
			THEN( "unserialized record is the same as original" ){
				REQUIRE(empty.reference() == record.reference());
				REQUIRE(empty.block() == record.block());
				REQUIRE(empty.trust() == record.trust());
				REQUIRE(empty.string(true) == record.string(true));
			}

			THEN( "unserialized record is valid" ){
				REQUIRE(empty.valid());
			}
		}
	}
}

