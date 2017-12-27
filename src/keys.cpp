#include <cryptopp/osrng.h>		// For AutoSeededRandomPool
#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace
#include <cryptopp/pssr.h>		// For PSSR
#include <cryptopp/whrlpool.h>	// For Whirlpool

#include <fstream>				// File I/O
#include <string>				// std::string

#include "keys.hpp"
#include "data.hpp"				// Data objects

/** The RSA key size */
#define KEY_SIZE 3072

using Signer   = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Signer;
using Verifier = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Verifier;

// -----------------------------------------------------------------------------
// PrivateKey Implementation

PrivateKey::PrivateKey( std::string fn ){
	this->load(fn);
}

void PrivateKey::generate(){
	CryptoPP::AutoSeededRandomPool rng;
	this->key.GenerateRandomWithKeySize(rng, KEY_SIZE);
}

void PrivateKey::sign( std::shared_ptr<Data> data ){
	std::string signature;
	Signer signer(this->key);

	CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::StringSource ss(data->to_string(), true,
				new CryptoPP::SignerFilter(rng, signer,
					new CryptoPP::HexEncoder(
						new CryptoPP::StringSink(signature))));

	data->set_signature(signature);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PublicKey Implementation

PublicKey::PublicKey( std::string fn ){
	this->load(fn);
}

void PublicKey::generate( std::shared_ptr<PrivateKey>& key ){
	CryptoPP::RSA::PublicKey n_key(key->get_key());
	this->key = n_key;
}

bool PublicKey::verify( std::shared_ptr<Data> data ){
	return this->verify( data.get() );
}

bool PublicKey::verify( Data* data ){
	std::string signature;
	bool result = false;
	Verifier verifier(this->key);

	CryptoPP::StringSource ss(data->get_signature(), true,
							  new CryptoPP::HexDecoder(
								new CryptoPP::StringSink(signature)));

	CryptoPP::StringSource ss2(signature + data->to_string(), true,
							   new CryptoPP::SignatureVerificationFilter(verifier,
								 new CryptoPP::ArraySink((byte*)&result,
														 sizeof(result))));
	return result;
}

// -----------------------------------------------------------------------------
