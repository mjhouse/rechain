/** \file	block.cpp
	\brief	Implements the Block class used to manage
			data stored in the BlockChain.
*/

#include "block.hpp"
#include "keys.hpp"

// cryptopp includes
#include <cryptopp/osrng.h>		// for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type
#include <cryptopp/hex.h>		// for the HexEncoder

// system includes
#include <string>
#include <iostream>
#include <sstream>

#include <chrono>
#include <climits>
#include <string>

#define MIN_NONCE 	1		/**< Minimum nonce value */
#define MAX_NONCE 	10000	/**< Maximum nonce value */

/** Maximum hash value (smaller increases difficulty) */
#define HASH_MAX  	"0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

/** Calculates the current timestamp in seconds
	since the epoch (Jan 1, 1970)
	\returns A Unix timestamp as a long
*/
long Block::get_epoch_timestamp(){
	// Get the time from the system
	auto tp = std::chrono::system_clock::now();

	// Get a duration since the epoch
	auto dur = tp.time_since_epoch();

	// Convert the duration to seconds
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(dur).count();

	// Typecast to long and return
	return (long)seconds;
}

/** Generates a random number between MIN_NONCE and MAX_NONCE
	\returns A random long betwen MIN_NONCE and MAX_NONCE
*/
long Block::get_nonce(){
	// Unwrap the namespace for convenience
	using CryptoPP::Integer;

	// Create a random generator
	CryptoPP::AutoSeededRandomPool rng;

	// Generate the random number, convert to long and return
	return Integer(rng,Integer(MIN_NONCE),Integer(MAX_NONCE)).ConvertToLong();
}

BlockType Block::type(){
	if(this->reference.empty()){
		return BlockType::DATA_BLOCK;
	} else {
		return BlockType::SIGNATURE_BLOCK;
	}
}

/*
	Get a string containing data in the block with
	the exception of the previous hash and the signature.
*/
std::string Block::signed_data(){
	// Create a string stream
	std::stringstream stream;

	// Feed the partial data to the stream
	stream	<< this->message
			<< this->public_key;

	// Return as a std::string
	return stream.str();
}


/*
	Get a string containing all data in the block
*/
std::string Block::full_data(){
	// Create a string stream
	std::stringstream stream;

	// Feed the signed data as well as
	// previous and the signature to the stream
	stream	<< this->signed_data()
			<< std::to_string(this->nonce)
			<< std::to_string(this->timestamp)
			<< std::to_string(this->counter)
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
	std::string digest;

	// Hash, hex encode and save to digest
	CryptoPP::StringSource s(this->full_data(),true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest))));

	// Return digest
	return digest;
}

/*
	Try to publish the block with a new nonce, timestamp and counter
*/
bool Block::publish(){
	// Make sure we don't overflow the max capacity of an unsigned int
	this->counter	= (this->counter >= UINT_MAX-1) ? 0 : this->counter + 1;

	// Update the nonce and timestamp
	this->nonce		 = this->get_nonce();
	this->timestamp  = this->get_epoch_timestamp();

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

	// Get the hash
	std::string hash = this->get_hash();

	// Check the signature
	bool sig_valid = this->is_signed();

	// Returns true if signature is good and
	// hash is valid
	return ( sig_valid && (hash <= HASH_MAX) );
}

/*
	Check that the block is signed correctly and the
	signature is valid.
*/
bool Block::is_signed(){

	// Check that there is a public key and signature
	if(!this->public_key.empty() && !this->signature.empty()){

		// Create a public key
		KeyPair keys;
		keys.set_public_key(this->public_key);

		// Check the signature
		return keys.verify(this->signed_data(),this->signature);
	}

	return false;
}
