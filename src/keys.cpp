/*
 * ReChain: The distributed research journal
 * Copyright (C) 2018  Michael House
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: michaelhouse@gmx.us
 *
*/

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

// Generate a new PrivateKey
void PrivateKey::generate(){
	CryptoPP::AutoSeededRandomPool rng;
	this->key.GenerateRandomWithKeySize(rng, KEY_SIZE);
}

// Sign a Data block
bool PrivateKey::sign( std::shared_ptr<Data> data ){
	if(data){
		std::string signature;

		// Create a public key and set it on the
		// Data object
		std::shared_ptr<PublicKey> pub_key(PublicKey::empty());
		pub_key->generate( shared_from_this() );
		data->set_public_key(pub_key->to_string());

		// Create a Signer and random generator
		Signer signer(this->key);
		CryptoPP::AutoSeededRandomPool rng;

		// Sign the Data object
		CryptoPP::StringSource ss(data->to_string(), true,
					new CryptoPP::SignerFilter(rng, signer,
						new CryptoPP::HexEncoder(
							new CryptoPP::StringSink(signature))));

		data->set_signature(signature);
		return true;
	}
	return false;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PublicKey Implementation

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
