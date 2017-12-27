/** \file	blockchain.cpp
	\brief	Implements the BlockChain class used to manage,
			load, save and publish any number of Block objects.
*/

#include "blockchain.hpp"
#include "block.hpp"
#include "keys.hpp"

#include <dependencies/cereal/types/memory.hpp>		/* For serializing smart pointers */
#include <dependencies/cereal/archives/json.hpp>	/* For the JSON archives */
#include <fstream>					/* File i/o */

/* Return the size of the BlockChain */
unsigned int BlockChain::size(){
	return this->chain.size();
}

bool BlockChain::publish( std::shared_ptr<Block> new_block ){

	if(new_block->is_signed()){
		// if the chain is empty, previous is empty (genesis block)
		// otherwise previous should point to the last block
		if(!this->chain.empty())
			new_block->set_previous(this->chain.back()->get_hash());
		else
			new_block->set_previous("");

		// Loop until the block is published
		while(!new_block->publish());

		// Add the new block to the chain
		this->chain.push_back(new_block);

		// Return success
		return true;
	}

	// Return failure
	return false;
}

/* Get a reference to a block */
std::shared_ptr<Block> BlockChain::get( int i ){
	std::shared_ptr<Block> ref;
	if( ((signed)this->chain.size() > i) && (i >= 0) ){
		ref = this->chain[i];
	}
	return ref;
}

/* Get a reference to a block */
std::shared_ptr<Block> BlockChain::get( std::string h ){
	std::shared_ptr<Block> ref;
	for( unsigned int i = 0; i < this->chain.size(); ++i ){
		if( h.compare(this->chain[i]->get_hash()) == 0 ){
			ref = this->chain[i];
		}
	}
	return ref;
}

/* Get trust calculation for each public key in the chain */
std::map<std::string,int> BlockChain::get_author_trust(){
	std::map<std::string,int> authors;
	std::map<std::string,std::string> blocks;

	// Put the key for the genesis block in the map
	std::string owner_key = this->chain[0]->get_public_key();
	authors.insert(std::make_pair(owner_key, 1));

	for(unsigned int i = 1; i < this->chain.size(); ++i ){
		// get the current block
		std::shared_ptr<Block> block = this->get(i);

		// Get the public key and save in the map if it
		// doesn't exist.
		std::string key = block->get_public_key();
		authors.insert(std::make_pair(key, 0));

		// If this is a signature block, get the trust
		// of the user and update the map.
		if( block->type() == BlockType::SIGNATURE_BLOCK ){
			std::string refkey	= blocks[ block->get_reference() ];
			int trust			= authors[key];
			if(refkey.compare(key)){
				authors[refkey] += trust;
			}
		} else {
			blocks.insert(std::make_pair(block->get_hash(), block->get_public_key()));
		}
	}

	return authors;
}

/* Get a reference to the last block */
std::shared_ptr<Block> BlockChain::back(){
	std::shared_ptr<Block> ref;
	if(!this->chain.empty()){
		ref = this->chain.back();
	}
	return ref;
}

/*
	If the BlockChain has a filename associated with it
	already, load from there.
*/
bool BlockChain::load(){
	// Check for a filename
	if(!this->filename.empty()){

		// Load from the saved filename and return
		return this->load(this->filename);
	}

	// Return failure
	return false;
}

/*
	Open the given file path and load blockchain data
	from it
*/
bool BlockChain::load( std::string fn ){
	// Open the file
	std::ifstream ifs(fn);

	// Save the file path
	this->filename = fn;

	// Check that the file is open
	if( ifs.is_open() ){

		// Create an Archive for it
		cereal::JSONInputArchive iarchive(ifs);

		// Load data from the archive into the BlockChain
		iarchive(*this);

		// Return success
		return true;
	}

	// Otherwise return failure
	return false;
}

/*
	If the BlockChain has a filename associated with it
	already, save there.
*/
bool BlockChain::save(){
	// Check for a filename
	if(!this->filename.empty()){

		// Load from the saved filename and return
		return this->save(this->filename);
	}

	// Return failure
	return false;
}

/*
	Save the BlockChain to the given file path
*/
bool BlockChain::save( std::string fn ){
	// Open the file
	std::ofstream ofs(fn);

	// Save the file path
	this->filename = fn;

	// Check that the file is open
	if( ofs.is_open() ){

		// Create an Archive for the file
		cereal::JSONOutputArchive oarchive(ofs);

		// Load data from the archive itno the BlockChain
		oarchive(*this);

		// Return success
		return true;
	}

	// Otherwise return failure
	return false;
}

/* Create a new genesis block with a public key
*/
std::shared_ptr<Block> BlockChain::create_genesis_block( KeyPair key ){
	// Create a shared_ptr for the block
	std::shared_ptr<Block> block;

	// Should only create a genesis block if
	// none exists
	if(this->chain.empty()){
		// Set the shared_ptr to a new block
		block.reset(new Block());

		// Sign the genesis block
		key.sign( block );

		// Publish and return a shared_ptr
		this->publish( block );
		return this->back();
	}

	// Otherwise return the empty shared_ptr
	return block;
}

/* Create a new data block with a public key and data
*/
std::shared_ptr<Block> BlockChain::create_data_block( std::string data, KeyPair key ){
	// Create a new block
	std::shared_ptr<Block> block(new Block());

	// Set the message and sign it
	block->set_message(data);
	key.sign( block );

	// Publish and return the new data block
	this->publish( block );
	return this->back();
}

/* Create a new signature block with a public key and reference
*/
std::shared_ptr<Block> BlockChain::create_signature_block( std::shared_ptr<Block> ref, KeyPair key ){
	// Create a new block
	std::shared_ptr<Block> block(new Block());

	// Set the refereence to the hash of the ref block
	// and sign with the key
	block->set_reference(ref->get_hash());
	key.sign( block );

	// Publish and return the new signature block
	this->publish( block );
	return this->back();
}
