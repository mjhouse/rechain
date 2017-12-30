/** \file	keypair.cpp
	\brief	Implements the KeyPair class used to manage,
			load, save and generate a public and
			private pair of RSA256 keys.
*/

#include "keypair.hpp"
#include "data.hpp"

#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/rsa.h>
#include <cryptopp/pssr.h>
#include <cryptopp/whrlpool.h>

#include <fstream>
#include <streambuf>
#include <exception>

/** Size of RSA keys to generate */
#define KEY_SIZE 3072

using Signer   = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Signer;	 /**< Simplify Signer */
using Verifier = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Verifier; /**< Simplify Verifier */

// -----------------------------------------------------------------------------
// Private Functions

std::string KeyPair::get_private_key(){

	/* Encode the private key */
	std::string key;
	this->private_key.Save(CryptoPP::HexEncoder(
								new CryptoPP::StringSink(key)).Ref());
	/* Return the encoded key */
	return key;
}

void KeyPair::set_private_key( std::string key ){
	/* Load the private key from the string */
	this->private_key.Load(CryptoPP::StringSource(key, true,
								new CryptoPP::HexDecoder()).Ref());
}

std::string KeyPair::get_public_key(){

	/* Encode the public key */
	std::string key;
	this->public_key.Save(CryptoPP::HexEncoder(
								new CryptoPP::StringSink(key)).Ref());
	/* Return the encoded key */
	return key;
}

void KeyPair::set_public_key( std::string key ){
	/* Load the public key from the string */
	this->public_key.Load(CryptoPP::StringSource(key, true,
								new CryptoPP::HexDecoder()).Ref());
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Public Functions

/*
	Attempt to save a key pair (*.private and *.public) at
	location 'fn'.
*/
bool KeyPair::save( std::string pub_f, std::string priv_f ){
	/* Open a file handle for each key */
	std::ofstream pri_file(priv_f);
	std::ofstream pub_file(pub_f);

	/* Check that both files are open */
	if( pri_file.is_open() && pub_file.is_open() ){

		/* Write the public and private keys to files */
		pub_file << this->get_public_key();
		pri_file << this->get_private_key();

		/* Return success */
		return true;
	}

	/* Return failure */
	return false;
}

/*
	Attempt to load a private key from a file path.
*/
bool KeyPair::load_private_key( std::string fn ){
	/* Open a file handle for the key */
	std::ifstream pri_file(fn);

	if( pri_file.is_open() ){
		/* Read the private key into a string */
		std::string privateKey = std::string((std::istreambuf_iterator<char>(pri_file)),
												std::istreambuf_iterator<char>());

		/* Load the private key from the string*/
		this->set_private_key(privateKey);

		/* Return success */
		return true;
	}

	/* Return failure */
	return false;
}

bool KeyPair::load_public_key( std::string fn ){
	/* Open a file handle for the key */
	std::ifstream pub_file(fn);

	if( pub_file.is_open() ){
		/* Read the private key into a string */
		std::string publicKey = std::string((std::istreambuf_iterator<char>(pub_file)),
												std::istreambuf_iterator<char>());

		/* Load the private key from the string*/
		this->set_public_key(publicKey);

		/* Return success */
		return true;
	}

	/* Return failure */
	return false;
}

/*
	Attempt to generate a key pair.
*/
bool KeyPair::generate(){
	try {
		/* Create a random generator */
		CryptoPP::AutoSeededRandomPool rng;

		/*
			Declare and generate a private key using
			the random pool and KEY_SIZE
		*/
		CryptoPP::RSA::PrivateKey privateKey;
		privateKey.GenerateRandomWithKeySize(rng, KEY_SIZE);

		/* Create a public key using the generated private key */
		CryptoPP::RSA::PublicKey publicKey(privateKey);

		/* Save the public/private keys */
		this->public_key = publicKey;
		this->private_key = privateKey;

		/* Return success */
		return true;
	} catch (std::exception& e){
		/*
			If an exception is thrown, return failure.
		*/
		return false;
	}
}

/*
	Sign a given string of data.
*/
std::string KeyPair::sign( std::shared_ptr<Data> data ){

	/* Create a string to hold the signature */
	std::string signature;

	/* Update the public key on the Data block */
	data->set_public_key( this->get_public_key() );

	/* Create Signer using the private_key */
	Signer signer(this->private_key);

	/* Initialize the random generator */
	CryptoPP::AutoSeededRandomPool rng;

	/* Sign and hex-encode the string */
	CryptoPP::StringSource ss(data->to_string(), true,
				new CryptoPP::SignerFilter(rng, signer,
					new CryptoPP::HexEncoder(
						new CryptoPP::StringSink(signature))));

	/* Set the signature on the Data */
	data->set_signature(signature);

	/* Return the result */
	return signature;
}

/*
	Verify a signature for a given data string.
*/
bool KeyPair::verify( std::shared_ptr<Data> data ){

	/* Create a string source that can decode the signature */
	std::string sig;
	CryptoPP::StringSource ss(data->get_signature(), true,
							  new CryptoPP::HexDecoder(
								new CryptoPP::StringSink(sig)));

	/* Init result to false */
	bool result = false;

	/* Verify the signature (updating 'result') */
	Verifier verifier(this->public_key);
	CryptoPP::StringSource ss2(sig + data->to_string(), true,
							   new CryptoPP::SignatureVerificationFilter(verifier,
								 new CryptoPP::ArraySink((byte*)&result,
														 sizeof(result))));

	/* Return result */
	return result;
}
