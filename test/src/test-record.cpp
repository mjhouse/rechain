#include "catch.hpp"
#include "block.hpp"
#include "record.hpp"
#include "keys.hpp"

#include <cryptopp/osrng.h>		// For AutoSeededRandomPool
#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace

#include <cereal/archives/json.hpp>

#include <string>

extern std::string generate_hash();
extern std::string get_path( std::string partial );
extern std::string dump_file( std::string path );

SCENARIO( "records created with different initial values", "[record-create]" ){

	GIVEN( "a reference and block hash" ){

		std::string reference = generate_hash();
		std::string block = generate_hash();

		WHEN( "record is created with only a reference" ){
			Record r(reference,"");

			THEN( "it reports the correct reference and type" ){
				REQUIRE(r.type() == DataType::Publication);
				REQUIRE(r.get_reference() == reference);
				REQUIRE(r.get_block() == "");
			}
		}

		WHEN( "record is created with reference and block hash" ){

			Record r(reference,block);

			THEN( "it reports the correct values and type" ){
				REQUIRE(r.get_reference() == reference);
				REQUIRE(r.get_block() == block);
				REQUIRE(r.type() == DataType::Signature);
			}
		}

		WHEN( "record is created with file iostream" ){
			std::string tmp_path = get_path("files/general/test_create_record.txt");
			Record r(tmp_path);

			CryptoPP::SHA256 hasher;
	
			std::string data = dump_file(tmp_path);
			std::string hash;

			CryptoPP::StringSource ss(data,true,
				new CryptoPP::HashFilter(hasher,
					new CryptoPP::HexEncoder(
						new CryptoPP::StringSink(hash)))); 

			THEN( "it reports the correct values and type" ){
				REQUIRE(r.get_reference() == hash);
				REQUIRE(r.type() == DataType::Publication);
			}
		}

	}

}

SCENARIO( "record methods are used to set and retrieve values", "[record-access]" ){

	GIVEN( "a publication and signature record" ){
		std::string reference = generate_hash();
		std::string block = generate_hash();

		Record publication(reference,"");
		Record signature(reference,block);
	

		WHEN( "records are signed" ){
			std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));
			private_key->sign(publication);

			THEN( "signed records are valid, unsigned are not" ){
				REQUIRE(publication.valid());
				REQUIRE_FALSE(signature.valid());
			}
		}

		WHEN( "values are retrieved" ){
			std::string r = publication.get_reference();
			std::string b = signature.get_block();

			THEN( "reference matches given reference" ){
				REQUIRE(r == reference);
			}

			THEN( "block reference matches given block reference" ){
				REQUIRE(b == block);
			}

			THEN( "types match expected values" ){
				REQUIRE(publication.type() == DataType::Publication);
				REQUIRE(signature.type() == DataType::Signature);
			}
		}

		WHEN( "values are set" ){
			publication.set_reference("TEST");
			signature.set_block("TEST");

			std::string r = publication.get_reference();
			std::string b = signature.get_block();

			THEN( "reference matches given reference" ){
				REQUIRE(r == "TEST");
			}

			THEN( "block reference matches given block reference" ){
				REQUIRE(b == "TEST");
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
		
		std::string reference = generate_hash();
		std::string block = generate_hash();

		Record record(reference,block);

		std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/test.private")));
		private_key->sign(record);

		WHEN( "record is converted to a string" ){
			std::string automatic = record.string(true);
			std::string manual;

			manual.append(record.get_reference());
			manual.append(record.get_block());
			manual.append(record.get_public_key());
			manual.append(record.get_signature());

			THEN( "string is equal to expected string" ){
				REQUIRE(manual == automatic);
			}
		}

		WHEN( "record is serialized to a file" ){
			Record empty;
			std::string tmp_path = get_path("files/general/tmp.record");
			std::ofstream ofs(tmp_path);
			if(ofs.is_open()){
				cereal::JSONOutputArchive archive(ofs);
				archive( record );
			}


			std::ifstream ifs(tmp_path);
			if(ifs.is_open()){
				cereal::JSONInputArchive archive(ifs);
				archive( empty );
			}

			std::remove(tmp_path.c_str());
			THEN( "unserialized record is the same as original" ){
				REQUIRE(empty.get_reference() == record.get_reference());
				REQUIRE(empty.get_block() == record.get_block());
				REQUIRE(empty.string(true) == record.string(true));
			}

			THEN( "unserialized record is valid" ){
				REQUIRE(empty.valid());
			}
		}
	}
}

