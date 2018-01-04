#include "catch.hpp"
#include "data.hpp"
#include "keys.hpp"

#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <sstream>
#include <fstream>

TEST_CASE( "data object tests", "[data]" ){
	Address address("DATA_REF","BLOCK_REF",DataType::Signature);
	std::shared_ptr<Data> data(new Data(address));
	std::shared_ptr<PrivateKey> key(PrivateKey::load_file("test/data/test.private"));

	std::ifstream f("test/data/test.public");
	if(f.is_open()){
		std::stringstream contents;
		contents << f.rdbuf();
		std::string pub_key = contents.str();

		data->set_public_key(pub_key);
	}

	key->sign(data);

	SECTION( "data object can set/get address" ) {
		std::shared_ptr<Data> d(new Data());
		d->set_address(address);

		Address a = d->get_address();

		REQUIRE(std::get<0>(a) == std::get<0>(address));
		REQUIRE(std::get<1>(a) == std::get<1>(address));
		REQUIRE(std::get<2>(a) == std::get<2>(address));
	}
	SECTION( "data object can set/get public key" ){
		std::ifstream f("test/data/test.public");
		REQUIRE(f.is_open());

		std::stringstream contents;
		contents << f.rdbuf();
		std::string pub_key = contents.str();

		data->set_public_key(pub_key);
		REQUIRE(data->get_public_key() == pub_key);
	}
	SECTION( "data object can set/get a signature" ){
		std::string s = "TEST";
		data->set_signature(s);
		REQUIRE(data->get_signature() == s);
	}
	SECTION( "data object can set/get a data reference" ){
		std::string s = "TEST";
		data->set_data_ref(s);
		REQUIRE(data->get_data_ref() == s);
	}
	SECTION( "data object can set/get a block reference" ){
		std::string s = "TEST";
		data->set_block_ref(s);
		REQUIRE(data->get_block_ref() == s);
	}
	SECTION( "data object can set/get a data type" ){
		data->set_data_type(DataType::Publication);
		REQUIRE(data->get_data_type() == DataType::Publication);
	}
	SECTION( "data object can return all data as a string" ){
		std::string full = data->to_string(true);
		std::string part = data->to_string();

		Address a = data->get_address();

		std::string manual_full;
		std::string manual_part;

		manual_part.append(std::get<0>(a));
		manual_part.append(std::get<1>(a));
		manual_part.append(std::to_string(std::get<2>(a)));
		manual_part.append(data->get_public_key());

		manual_full.append(manual_part);
		manual_full.append(data->get_signature());
		manual_full.append(std::to_string(data->get_trust()));

		REQUIRE(full == manual_full);
		REQUIRE(part == manual_part);
	}
	SECTION( "data object that should be valid is verified" ){
		REQUIRE(data->verify());
	}
}
