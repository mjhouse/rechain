// dependency includes
#include <cryptopp/osrng.h>	// for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type
#include <cryptopp/hex.h>	// for the HexEncoder

// system includes
#include <climits>
#include <algorithm>
#include <iostream>

// local includes
#include "data.hpp"
#include "block.hpp"

/* Get the hash of this block
*/
std::string Block::hash(){
	std::string hash_data;

	// Get the hashable data for each Data block
	std::for_each( this->data.begin(), this->data.end(),
	[&hash_data]( std::pair<std::string,std::shared_ptr<Data>> el ){
		hash_data.append( el.second->to_string(true) );
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
void Block::change_hash(){
	// Update the counter, and reset to 0 if it
	// gets to the int max
	if(this->counter >= UINT_MAX-1) this->counter = 0;
	else this->counter++;

	// Update the nonce and timestamp
	this->nonce	= new_nonce();
	this->timestamp = new_timestamp();
}

/* Get a data block given the signature
*/
std::shared_ptr<Data> Block::get_data( std::string s ){
	// Create a new shared_ptr and try to
	// get an iterator to the appropriate map
	// pair.
	std::shared_ptr<Data> d;
	auto it = this->data.find(s);

	// If the iterator points to an
	// entry, point the shared_ptr at it.
	if(it != this->data.end())
		d = it->second;

	// Return d
	return d;
}

/* Add a Data block
*/
bool Block::add_data( std::shared_ptr<Data> d ){
	// Check the Data object is signed and
	// valid.
	if(d->verify()){
		// Try to insert the new Data pointer
		auto result = this->data.insert(std::pair<std::string,std::shared_ptr<Data>>( d->get_signature(), d ));

		// Return the results of the insertion
		return result.second;
	}
	// Return failure if the Data pointer
	// isn't valid.
	return false;
}

/* Remove a Data block
*/
bool Block::remove_data( std::string s ){
	if(this->data.erase(s) > 0)
		return true;
	return false;
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
