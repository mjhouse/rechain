/** \file	blockchain.cpp
	\brief	Implements the BlockChain class used to manage,
			load, save and publish any number of BasicBlock objects.
*/

#include "blockchain.hpp"
#include "basic_block.hpp"
#include "keys.hpp"

#include <dependencies/cereal/types/memory.hpp>		/* For serializing smart pointers */
#include <dependencies/cereal/archives/json.hpp>	/* For the JSON archives */
#include <fstream>					/* File i/o */

/* Publish a new block */
bool BlockChain::publish( std::shared_ptr<BasicBlock> new_block ){

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
std::shared_ptr<BasicBlock> BlockChain::get( unsigned int i ){
	std::shared_ptr<BasicBlock> ref;
	if( this->chain.size() > i ){
		ref = this->chain[i];
	}
	return ref;
}

/* Get a reference to a block */
std::shared_ptr<BasicBlock> BlockChain::get( std::string h ){
	std::shared_ptr<BasicBlock> ref;
	for( unsigned int i = 0; i < this->chain.size(); ++i ){
		if( h.compare(this->chain[i]->get_hash()) == 0 ){
			ref = this->chain[i];
		}
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
