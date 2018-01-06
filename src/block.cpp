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
#include <iostream>
#include <vector>

// local includes
#include "data.hpp"
#include "block.hpp"

Block::~Block(){
	for(auto d : this->data) delete d;
	this->data.clear();
}

/* Get the hash of this block
*/
std::string Block::hash(){
	std::string hash_data;

	// Get the hashable data for each Data block
	std::for_each( this->data.begin(), this->data.end(),
	[&hash_data]( Data* d ){
		hash_data.append( d->to_string(true) );
	});

	hash_data.append( this->previous );
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

		// Update the nonce and timestamp
		this->nonce	= new_nonce();
		this->timestamp = new_timestamp();	
	}

	return this->hash();
	
}

/* Get a Data block given the signature
*/
Data* Block::get_data( std::string s ){
	// Iterate through data and find the 
	// matching signature
	for(auto d : this->data){
		if(d->get_signature() == s) return d;
	}

	return nullptr;
}

/* Update the current trust of each signature
*/
void Block::set_trust( std::map<std::string,float> trust ){
	for(auto d : this->data){
		if(d->get_data_type() == DataType::Signature){
			float t = trust[d->get_public_key()];
			d->set_trust(t);
		}
	}
}

/* Get a Data block given the index
*/
Data* Block::get_data( unsigned int i ){
	if(i < this->data.size()) return this->data[i];
	return nullptr;
}

/* Get all Data objects
*/
std::vector<Data*> Block::get_data(){
	return this->data;
}

/* Add a Data block
*/
bool Block::add_data( Data* d ){
	// Check the Data object is signed and
	// valid.
	if(d->verify()){
		// Check to make sure the signature is unique
		for(auto i : this->data){
			if(i->get_signature() == d->get_signature()) return false;
		}
	
		// Add to the end of data	
		this->data.push_back(d);
		return true;

	}
	// Return failure if the Data pointer
	// isn't valid.
	return false;
}

/* Remove a Data block
*/
void Block::remove_data( std::string s ){
	// Erase Data objects with the given signature
	this->data.erase( std::remove_if(this->data.begin(),this->data.end(),
		[&s]( Data* d ){
			return (d->get_signature() == s);
		}
	));
}

/** Get the hash of the previous block
*/
std::string Block::get_previous(){ return this->previous; }


/** Set the hash of the previous block
*/
void Block::set_previous( std::string h ){ this->previous = h; }

/* Return the size of the Block
*/
size_t Block::size(){
	return this->data.size();
}
