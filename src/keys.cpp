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
 * Contact: mjhouse@protonmail.com
 *
*/

#include <cryptopp/osrng.h>		// For AutoSeededRandomPool
#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace
#include <cryptopp/pssr.h>		// For PSSR
#include <cryptopp/whrlpool.h>		// For Whirlpool

#include <fstream>				// File I/O
#include <string>				// std::string

#include "keys.hpp"
#include "logger.hpp"

/** The RSA key size */
#define KEY_SIZE 3072

using Signer   = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Signer;
using Verifier = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Verifier;

// -----------------------------------------------------------------------------
// PrivateKey Implementation

// Generate a new PrivateKey
void PrivateKey::generate(){
	CryptoPP::AutoSeededRandomPool rng;
	key.GenerateRandomWithKeySize(rng, KEY_SIZE);
}

PublicKey* PrivateKey::get_public(){

    PublicKey* public_key = new PublicKey();
    public_key->generate(this);

    return public_key;

}

void PrivateKey::sign( BaseRecord* t_record ){
	std::string signature;

	// create a public key and set it on the  Record
	std::shared_ptr<PublicKey> pub_key(PublicKey::empty());
	pub_key->generate( this );

	t_record->set_public_key(pub_key->to_string());

	// create a Signer and random generator
	Signer signer(key);
	CryptoPP::AutoSeededRandomPool rng;

	// sign the Data object
	CryptoPP::StringSource ss(t_record->get_data(), true,
				new CryptoPP::SignerFilter(rng, signer,
					new CryptoPP::HexEncoder(
						new CryptoPP::StringSink(signature))));

	t_record->set_signature(signature);
}

// Sign a Record
void PrivateKey::sign( std::shared_ptr<BaseRecord> t_record ){
    sign(t_record.get());
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PublicKey Implementation

// Generate a new PublicKey from a PrivateKey
void PublicKey::generate( PrivateKey* t_key ){
	CryptoPP::RSA::PublicKey n_key(t_key->get_key());
	key = n_key;
}

// Verify the signature on a Data block
bool PublicKey::verify( BaseRecord* t_record ){
	std::string signature;
	bool result = false;

	Verifier verifier(key);

	CryptoPP::StringSource ss(t_record->get_signature(), true,
				  new CryptoPP::HexDecoder(
					new CryptoPP::StringSink(signature)));

	CryptoPP::StringSource ss2(signature + t_record->get_data(), true,
				   new CryptoPP::SignatureVerificationFilter(verifier,
					 new CryptoPP::ArraySink((unsigned char*)&result, sizeof(result))));

	return result;
}

// -----------------------------------------------------------------------------
