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

// dependency includes
#include <cryptopp/osrng.h>	// for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type
#include <cryptopp/hex.h>	// for the HexEncoder

// system includes
#include <climits>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

// local includes
#include "record.hpp"
#include "block.hpp"
#include "logger.hpp"

Block::~Block(){}

using rl = Logger;

/* Get the hash of this block
*/
std::string Block::hash(){
	std::string hash_data;

	// Get the hashable data for each Record block
	std::for_each( records.begin(), records.end(),
	[&hash_data]( Record r ){
		hash_data.append( r.string(true) );
	});

	hash_data.append( this->prev );
	hash_data.append( std::to_string(this->nonce) );
	hash_data.append( std::to_string(this->timestamp) );
	hash_data.append( std::to_string(this->counter) );

	// Create a hash
	CryptoPP::SHA256 hasher;
	std::string new_hash;

	// Hash, hex encode and save to new_hash
	CryptoPP::StringSource ss(hash_data,true,
		new CryptoPP::HashFilter(hasher,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(new_hash))));
	// Return new_hash
	return new_hash;
}

/* Change the hashing variables
*/
std::string Block::mine(){

	// Mine the block until it has a valid hash
	while(this->hash() > HASH_MAX){
		// Update the counter, and reset to 0 if it
		// gets to the int max
		if(this->counter >= UINT_MAX-1) this->counter = 0;
		else this->counter++;

		// Update the timestamp
		auto e = std::chrono::system_clock::now().time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(e).count();
		this->timestamp = (long)seconds;

		// Update the random nonce
		CryptoPP::AutoSeededRandomPool rng;
		this->nonce = CryptoPP::Integer(rng,
			CryptoPP::Integer(1),
			CryptoPP::Integer(LONG_MAX)).ConvertToLong();
	}

	return this->hash();

}

/* Check if Block is valid
*/
bool Block::valid(){
	// Make sure all records are valid
	for(auto r : records)
		if(!r.valid())
			return false;

	// Check that hash is good
	if(this->hash() > HASH_MAX)
		return false;

	// The Block is good
	return true;
}

/** Overloaded index operator
*/
Record& Block::operator[] ( unsigned int i ){
	return records[i];
}

/** Overloaded assignment operator
*/
Block& Block::operator=( const Block& b ){
	records		= b.records;
	prev		= b.prev;
	nonce		= b.nonce;
	timestamp	= b.timestamp;
	counter		= b.counter;
	return *this;
}
/* Get an iterator to a Record block given the signature
*/
Block::iterator Block::find( std::string s ){
	return std::find_if(records.begin(),records.end(),
	[&s](Record& r){ return (r.reference() == s); });
}

/* Add a Record block
*/
bool Block::add( Record& r ){
	// Check the Record object is signed and
	// valid.
	if(r.valid()){
		// Add to the end of data
		records.push_back(r);
		return true;
	}
	// Return failure if the Record pointer
	// isn't valid.
	return false;
}

/** Get or set the hash of the previous block
*/
std::string Block::previous( std::string h ){
	if(!h.empty()) this->prev = h;
	return this->prev;
}

/** Return an iterator to the start of the Record
*/
Block::iterator Block::begin(){
	return records.begin();
}

/** Returns an iterator to the end of the Record collection
*/
Block::iterator Block::end(){
	return records.end();
}

/* Return the size of the Block
*/
size_t Block::size(){
	return records.size();
}
