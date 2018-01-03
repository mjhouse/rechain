#include "catch.hpp"
#include "keys.hpp"
#include "data.hpp"

#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <sstream>
#include <fstream>

std::string _read_file( std::string fn ){
	std::ifstream f(fn);
	if(f.is_open()){
		std::stringstream contents;
		contents << f.rdbuf();
		return contents.str();
	}
	return "";
}

TEST_CASE( "base key tests for privatekey and publickey", "[basekey]" ){
	std::string pri_fn	= "test/data/test.private";
	std::string pub_fn	= "test/data/test.public";

	std::string pri_key = _read_file(pri_fn);
	std::string pub_key = _read_file(pub_fn);

	std::shared_ptr<PrivateKey> private_key(PrivateKey::empty());
	std::shared_ptr<PublicKey> public_key(PublicKey::empty());

	SECTION( "can get internal key" ) {
		private_key->from_string(pri_key);
		public_key->from_string(pub_key);
		CryptoPP::RSA::PrivateKey c_pri_key = private_key->get_key();
		CryptoPP::RSA::PublicKey c_pub_key = public_key->get_key();

		CryptoPP::AutoSeededRandomPool rng;

		REQUIRE(c_pub_key.Validate(rng,2));
		REQUIRE(c_pri_key.Validate(rng,2));
	}
	SECTION( "can set internal key" ) {
		CryptoPP::RSA::PrivateKey c_pri_key;
		c_pri_key.Load(CryptoPP::StringSource(pri_key, true, new CryptoPP::HexDecoder()).Ref());

		CryptoPP::RSA::PublicKey c_pub_key;
		c_pub_key.Load(CryptoPP::StringSource(pub_key, true, new CryptoPP::HexDecoder()).Ref());

		private_key->set_key(c_pri_key);
		public_key->set_key(c_pub_key);

		CryptoPP::AutoSeededRandomPool rng;
		REQUIRE(private_key->get_key().Validate(rng,2));
		REQUIRE(public_key->get_key().Validate(rng,2));
	}
	SECTION( "keys can be converted from/to a string" ) {
		private_key->from_string(pri_key);
		public_key->from_string(pub_key);

		CryptoPP::AutoSeededRandomPool rng;
		REQUIRE(private_key->get_key().Validate(rng,2));
		REQUIRE(public_key->get_key().Validate(rng,2));
		REQUIRE(private_key->to_string() == pri_key);
		REQUIRE(public_key->to_string() == pub_key);
	}
	SECTION( "keys can be loaded from file" ) {
		bool result1 = private_key->load(pri_fn);
		bool result2 = public_key->load(pub_fn);
		CryptoPP::AutoSeededRandomPool rng;

		REQUIRE(result1);
		REQUIRE(result2);
		REQUIRE(private_key->get_key().Validate(rng,2));
		REQUIRE(public_key->get_key().Validate(rng,2));
		REQUIRE(private_key->to_string() == pri_key);
		REQUIRE(public_key->to_string() == pub_key);
	}
	SECTION( "keys cannot be loaded from bad filename" ){
		bool result1 = private_key->load( "BAD PATH" );
		bool result2 = public_key->load( "BAD PATH" );

		REQUIRE_FALSE(result1);
		REQUIRE_FALSE(result2);
	}
	SECTION( "keys can be saved to a file" ) {
		private_key->load(pri_fn);
		public_key->load(pub_fn);

		bool result1 = private_key->save("test/data/save_test.private");
		bool result2 = public_key->save("test/data/save_test.public");
		std::string s_pri = _read_file("test/data/save_test.private");
		std::string s_pub = _read_file("test/data/save_test.public");

		REQUIRE(result1);
		REQUIRE(result2);
		REQUIRE(s_pri == private_key->to_string());
		REQUIRE(pri_key == private_key->to_string());
		REQUIRE(s_pub == public_key->to_string());
		REQUIRE(pub_key == public_key->to_string());

		std::remove("test/data/save_test.private");
		std::remove("test/data/save_test.public");
	}
	SECTION( "keys cannot be saved to a bad filename" ){
		private_key->load(pri_fn);
		public_key->load(pub_fn);

		bool result1 = private_key->save("test/NOEXIST/save_test.private");
		bool result2 = public_key->save("test/NOEXIST/save_test.public");

		REQUIRE_FALSE(result1);
		REQUIRE_FALSE(result2);
	}
}

TEST_CASE( "private key tests", "[privatekey]" ){
	std::shared_ptr<PrivateKey> key(PrivateKey::empty());
	std::shared_ptr<Data> data(new Data(Address("DATA_REF","BLOCK_REF",DataType::Signature)));

	std::string pri_fn	= "test/data/test.private";
	std::string pri_key = _read_file(pri_fn);

	SECTION( "private key can be created from a string" ) {
		std::shared_ptr<PrivateKey> k(PrivateKey::load_string(pri_key));

		CryptoPP::AutoSeededRandomPool rng;
		REQUIRE(k->get_key().Validate(rng,2));
		REQUIRE(!k->to_string().empty());
		REQUIRE(k->to_string() == pri_key);
	}
	SECTION( "private key can be created from file" ) {
		std::shared_ptr<PrivateKey> k(PrivateKey::load_file(pri_fn));

		CryptoPP::AutoSeededRandomPool rng;
		REQUIRE(k->get_key().Validate(rng,2));
		REQUIRE(!k->to_string().empty());
		REQUIRE(k->to_string() == pri_key);
	}
	SECTION( "private key generates a valid key" ) {
		key->generate();
		CryptoPP::AutoSeededRandomPool rng;

		REQUIRE(!key->to_string().empty());
		REQUIRE(key->get_key().Validate(rng,2));
	}
	SECTION( "private key signs a Data object" ) {
		key->from_string(pri_key);
		key->sign(data);

		REQUIRE(data->verify());
	}
	SECTION( "private key won't sign bad pointer" ){
		std::shared_ptr<Data> d;
		bool signed_ = key->sign(d);

		REQUIRE_FALSE(signed_);
	}

}

TEST_CASE( "public key tests", "[publickey]" ){
	std::shared_ptr<Data> data(new Data(Address("DATA_REF","BLOCK_REF",DataType::Signature)));

	std::string pri_fn	= "test/data/test.private";
	std::string pub_fn	= "test/data/test.public";

	std::string pri_key = _read_file(pri_fn);
	std::string pub_key = _read_file(pub_fn);

	std::shared_ptr<PrivateKey> private_key(PrivateKey::load_string(pri_key));
	std::shared_ptr<PublicKey> key(PublicKey::load_string(pub_key));

	SECTION( "public key can be created from a string" ) {
		std::shared_ptr<PublicKey> k(PublicKey::load_string(pub_key));

		CryptoPP::AutoSeededRandomPool rng;
		REQUIRE(k->get_key().Validate(rng,2));
		REQUIRE(!k->to_string().empty());
		REQUIRE(k->to_string() == pub_key);
	}
	SECTION( "public key can be created from file" ) {
		std::shared_ptr<PublicKey> k(PublicKey::load_file(pub_fn));

		CryptoPP::AutoSeededRandomPool rng;
		REQUIRE(k->get_key().Validate(rng,2));
		REQUIRE(!k->to_string().empty());
		REQUIRE(k->to_string() == pub_key);
	}
	SECTION( "public key generates a valid key" ) {
		key->generate(private_key);
		CryptoPP::AutoSeededRandomPool rng;

		REQUIRE(!key->to_string().empty());
		REQUIRE(key->get_key().Validate(rng,2));
	}
	SECTION( "public key verifies a Data object" ) {
		bool signed_ = private_key->sign(data);
		bool verify_ = key->verify(data);

		REQUIRE(signed_);
		REQUIRE(verify_);
	}
	SECTION( "public key doesn't verify expired ptr" ){
		std::shared_ptr<Data> d;
		bool verify_ = key->verify(d);

		REQUIRE_FALSE(verify_);
	}
}
