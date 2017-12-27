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

// Constructor loads a PrivateKey from a file path
PrivateKey::PrivateKey( std::string fn ){
	this->load(fn);
}

// Generate a new PrivateKey
void PrivateKey::generate(){
	CryptoPP::AutoSeededRandomPool rng;
	this->key.GenerateRandomWithKeySize(rng, KEY_SIZE);
}

// Sign a Data block
void PrivateKey::sign( std::shared_ptr<Data> data ){
	if(data){
		std::string signature;

		// Create a public key and set it on the
		// Data object
		PublicKey pubkey;
		pubkey.generate( shared_from_this() );
		data->set_public_key(pubkey.to_string());

		// Create a Signer and random generator
		Signer signer(this->key);
		CryptoPP::AutoSeededRandomPool rng;

		// Sign the Data object
		CryptoPP::StringSource ss(data->to_string(), true,
					new CryptoPP::SignerFilter(rng, signer,
						new CryptoPP::HexEncoder(
							new CryptoPP::StringSink(signature))));

		data->set_signature(signature);

	} else { _OUT("Can't sign Data"); }
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PublicKey Implementation

// Constructor loads PublicKey from a file path
PublicKey::PublicKey( std::string fn ){
	this->load(fn);
}

// Generate a new PublicKey from a PrivateKey
void PublicKey::generate( std::shared_ptr<PrivateKey> key ){
	CryptoPP::RSA::PublicKey n_key(key->get_key());
	this->key = n_key;
}

// Verify the signature on a Data block
bool PublicKey::verify( std::shared_ptr<Data> data ){
	if(data){
		std::string signature;
		bool result = false;

		Verifier verifier(this->key);

		CryptoPP::StringSource ss(data->get_signature(), true,
								  new CryptoPP::HexDecoder(
									new CryptoPP::StringSink(signature)));

		CryptoPP::StringSource ss2(signature + data->to_string(), true,
								   new CryptoPP::SignatureVerificationFilter(verifier,
									 new CryptoPP::ArraySink((byte*)&result, sizeof(result))));
		return result;
	}
	return false;
}

// -----------------------------------------------------------------------------
