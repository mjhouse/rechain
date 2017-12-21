#include "block.hpp"
#include "keys.hpp"

#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/rsa.h>
#include <cryptopp/pssr.h>
#include <cryptopp/whrlpool.h>

#include <iostream>
#include <sstream>

#include <chrono>
#include <climits>
#include <string>

#define MIN_NONCE 	1
#define MAX_NONCE 	10000
#define HASH_MAX  	"0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

using Signer   = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Signer;
using Verifier = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Verifier;

long get_epoch_timestamp(){
	auto tp = std::chrono::system_clock::now();
	auto dur = tp.time_since_epoch();

	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
	return (long)seconds;
}

long get_nonce(){
	using CryptoPP::Integer;

	CryptoPP::AutoSeededRandomPool rng;
	return Integer(rng,Integer(MIN_NONCE),Integer(MAX_NONCE)).ConvertToLong();
}

std::string Block::partial_data(){
	std::stringstream stream;
	stream	<< this->message
			<< this->public_key
			<< this->reference
			<< std::to_string(this->nonce)
			<< std::to_string(this->timestamp)
			<< std::to_string(this->counter);
	return stream.str();
}

std::string Block::full_data(){
	std::stringstream stream;
	stream	<< this->partial_data()
			<< this->previous
			<< this->signature;
	return stream.str();
}

std::string Block::sign( KeyPair keys ){
	CryptoPP::RSA::PrivateKey private_key;
	private_key.Load(CryptoPP::StringSource(keys.get_private_key(), true,
										   new CryptoPP::HexDecoder()).Ref());
   std::string signature;
   Signer signer(private_key);
   CryptoPP::AutoSeededRandomPool rng;

   CryptoPP::StringSource ss(this->partial_data(), true,
							 new CryptoPP::SignerFilter(rng, signer,
							   new CryptoPP::HexEncoder(
								 new CryptoPP::StringSink(signature))));
	return signature;
}

bool Block::check_sig(){
	CryptoPP::RSA::PublicKey key;
	key.Load(CryptoPP::StringSource(this->public_key, true,
									new CryptoPP::HexDecoder()).Ref());
	std::string sig;
	CryptoPP::StringSource ss(this->signature, true,
							  new CryptoPP::HexDecoder(
								new CryptoPP::StringSink(sig)));

	bool result = false;
	Verifier verifier(key);
	CryptoPP::StringSource ss2(sig + this->partial_data(), true,
							   new CryptoPP::SignatureVerificationFilter(verifier,
								 new CryptoPP::ArraySink((byte*)&result,
														 sizeof(result))));

	return result;
}

std::string Block::get_hash(){
	CryptoPP::SHA256 hash;

	std::string data = this->full_data();
	std::string digest;

	CryptoPP::StringSource s(data,true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest))));

	return digest;
}

bool Block::publish( KeyPair keys ){
	// make sure we don't overflow the max capacity of an unsigned int
	this->counter	= (this->counter >= UINT_MAX-1) ? 0 : this->counter + 1;

	// update the nonce and timestamp
	this->nonce		 = get_nonce();
	this->timestamp  = get_epoch_timestamp();
	this->public_key = keys.get_public_key();
	this->signature  = this->sign( keys );

	// hash this block
	std::string hash = this->get_hash();

	// return whether the hash is good
	return (hash <= HASH_MAX);
}

bool Block::is_valid(){
	std::string hash = this->get_hash();
	bool sig_good	 = this->check_sig();

	return ( sig_good && (hash <= HASH_MAX) );
}

void Block::print(){
	std::cout << "--------------" << std::endl <<
		"	hash:       " << this->get_hash() << std::endl <<
		"	previous:   " << this->previous	 << std::endl <<
		"	public_key: " << this->public_key.substr(0,20) + "..." << std::endl <<
		"	signature:  " << this->signature	 << std::endl <<
		"	reference:  " << this->reference	 << std::endl <<
		"	message:    " << this->message	 << std::endl <<
		"	timestamp:  " << this->timestamp  << std::endl <<
		"	count:      " << this->counter	 << std::endl <<
		"	nonce:      " << this->nonce		 << std::endl <<
		"---------------" << std::endl;
}
