/** \file	block.cpp
	\brief	Implements the Block class used to manage
			data (publish, hash etc.) stored in the
			BlockChain.
*/

#include "block.hpp"
#include "keys.hpp"

// cryptopp includes
#include <cryptopp/osrng.h>		// for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type
#include <cryptopp/hex.h>		// for the HexEncoder

// system includes
#include <iostream>
#include <sstream>

#include <chrono>
#include <climits>
#include <string>

#define MIN_NONCE 	1		/* Minimum nonce value */
#define MAX_NONCE 	10000	/* Maximum nonce value */

/* Maximum hash value (smaller increases difficulty) */
#define HASH_MAX  	"0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

/*
	Calculates the current timestamp in seconds
	since the epoch (Jan 1, 1970)
*/
long get_epoch_timestamp(){
	// Get the time from the system
	auto tp = std::chrono::system_clock::now();

	// Get a duration since the epoch
	auto dur = tp.time_since_epoch();

	// Convert the duration to seconds
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(dur).count();

	// Typecast to long and return
	return (long)seconds;
}

/*
	Generates a random number between MIN_NONCE and MAX_NONCE
*/
long get_nonce(){
	// Unwrap the namespace for convenience
	using CryptoPP::Integer;

	// Create a random generator
	CryptoPP::AutoSeededRandomPool rng;

	// Generate the random number, convert to long and return
	return Integer(rng,Integer(MIN_NONCE),Integer(MAX_NONCE)).ConvertToLong();
}

/*
	Get a string containing data in the block with
	the exception of the previous hash and the signature.
*/
std::string Block::partial_data(){
	// Create a stringstream
	std::stringstream stream;

	// Feed each variable as a string
	// to the stream
	stream	<< this->message
			<< this->public_key
			<< this->reference
			<< std::to_string(this->nonce)
			<< std::to_string(this->timestamp)
			<< std::to_string(this->counter);

	// Return as a std::string
	return stream.str();
}

/*
	Get a string containing all data in the block
*/
std::string Block::full_data(){
	// Create a string stream
	std::stringstream stream;

	// Feed the partial data as well as
	// previous and the signature to the stream
	stream	<< this->partial_data()
			<< this->previous
			<< this->signature;

	// Return as a std::string
	return stream.str();
}

/*
	Get the hash of the data contained in this block
*/
std::string Block::get_hash(){
	// Create a hash
	CryptoPP::SHA256 hash;

	// Get the full data string
	std::string data = this->full_data();
	std::string digest;

	// Hash, hex encode and save to digest
	CryptoPP::StringSource s(data,true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest))));

	// Return digest
	return digest;
}

/*
	Try to publish the block with a new nonce, timestamp and counter
*/
bool Block::publish( KeyPair keys ){
	// Make sure we don't overflow the max capacity of an unsigned int
	this->counter	= (this->counter >= UINT_MAX-1) ? 0 : this->counter + 1;

	// Update the nonce and timestamp
	this->nonce		 = get_nonce();
	this->timestamp  = get_epoch_timestamp();

	// Get the public key and sign the data
	this->public_key = keys.get_public_key();
	this->signature  = keys.sign( this->partial_data() );

	// Hash this block
	std::string hash = this->get_hash();

	// Returns true if hash is valid
	return (hash <= HASH_MAX);
}

/*
	Check if the partial data matches the signature, the
	full data (with signature) matches the hash, and the
	hash is small enough.
*/
bool Block::is_valid(){
	// Create a public key
	KeyPair keys;
	keys.set_public_key(this->public_key);

	// Get the hash
	std::string hash = this->get_hash();

	// Check the signature
	bool sig_valid = keys.verify(this->partial_data(),this->signature);

	// Returns true if signature is good and
	// hash is valid
	return ( sig_valid && (hash <= HASH_MAX) );
}

/*
	Print function for debugging
*/
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
