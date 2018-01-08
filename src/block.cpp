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
#include "data.hpp"
#include "block.hpp"

Block::~Block(){
	this->data.clear();
}

/* Get the hash of this block
*/
std::string Block::hash(){
	std::string hash_data;

	// Get the hashable data for each Data block
	std::for_each( this->data.begin(), this->data.end(),
	[&hash_data]( Data d ){
		hash_data.append( d.to_string(true) );
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

/** Overloaded index operator
*/
Data& Block::operator[] ( unsigned int i ){
	return this->data[i];
}

/** Overloaded assignment operator
*/
Block& Block::operator=( const Block& b ){
	this->data	= b.data;
	this->prev	= b.prev;
	this->nonce	= b.nonce;
	this->timestamp	= b.timestamp;
	this->counter	= b.counter;
	return *this;
}
/* Get an iterator to a Data block given the signature
*/
Block::iterator Block::find( std::string s ){
	return std::find_if(this->data.begin(),this->data.end(),
	[&s](Data& d){
		return (d.get_signature() == s);
	});
}

/* Add a Data block
*/
bool Block::add( Data& d ){
	// Check the Data object is signed and
	// valid.
	if(d.verify()){
		// Add to the end of data	
		this->data.push_back(d);
		return true;

	}
	// Return failure if the Data pointer
	// isn't valid.
	return false;
}

/** Get or set the hash of the previous block
*/
std::string Block::previous( std::string h ){
	if(!h.empty()) this->prev = h;
	return this->prev;
}

/** Return an iterator to the start of the Data
*/
Block::iterator Block::begin(){ 
	return this->data.begin();
}

/** Returns an iterator to the end of the Data collection
*/ 
Block::iterator Block::end(){
	return this->data.end();
}

/* Return the size of the Block
*/
size_t Block::size(){
	return this->data.size();
}
