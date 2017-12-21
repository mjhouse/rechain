#include "keys.hpp"

#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/rsa.h>

#include <fstream>
#include <streambuf>
#include <exception>

#define KEY_SIZE 3072

KeyPair::KeyPair( std::string fn ){
	this->load(fn);
}

bool KeyPair::save( std::string fn ){
	std::ofstream pri_file(fn + ".private");
	std::ofstream pub_file(fn + ".public");

	if( pri_file && pub_file ){
		pub_file << this->public_key;
		pri_file << this->private_key;
		return true;
	}

	return false;
}

bool KeyPair::load( std::string fn ){
	std::ifstream pri_file(fn + ".private");
	std::ifstream pub_file(fn + ".public");

	if(pri_file&&pub_file){
		this->public_key = std::string((std::istreambuf_iterator<char>(pub_file)),
			std::istreambuf_iterator<char>());
		this->private_key = std::string((std::istreambuf_iterator<char>(pri_file)),
			std::istreambuf_iterator<char>());
		return true;
	}

	return false;
}

bool KeyPair::generate(){
	try {
		CryptoPP::AutoSeededRandomPool rng;

		CryptoPP::RSA::PrivateKey privateKey;
		privateKey.GenerateRandomWithKeySize(rng, KEY_SIZE);
		CryptoPP::RSA::PublicKey publicKey(privateKey);

		publicKey.Save( CryptoPP::HexEncoder(
						new CryptoPP::StringSink(this->public_key)).Ref());
		privateKey.Save(CryptoPP::HexEncoder(
						new CryptoPP::StringSink(this->private_key)).Ref());

		return true;
	} catch (std::exception& e){
		return false;
	}
}
