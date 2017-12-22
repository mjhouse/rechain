/** \file	blockchain.cpp
	\brief	Implements the BlockChain class used to manage,
			load, save and publish any number of Block objects.
*/

#include "blockchain.hpp"
#include "block.hpp"
#include "keys.hpp"

#include <cereal/types/memory.hpp>	/* For serializing smart pointers */
#include <cereal/archives/json.hpp>	/* For the JSON archives */
#include <fstream>					/* File i/o */

/* Publish a new block, signing it with the given keys */
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
std::weak_ptr<Block> BlockChain::get( unsigned int i ){
	std::weak_ptr<Block> ref;
	if( this->chain.size() > i ){
		ref = this->chain[i];
	}
	return ref;
}

/* Print for debugging */
void BlockChain::print(){
	std::cout << "-- BLOCKCHAIN --" << std::endl;
	for(unsigned int i = 0; i < this->chain.size(); ++i){
		std::cout << "BLOCK #" << i << std::endl;
		this->chain[i]->print();
	}
}

/*
	Open the given file path and load blockchain data
	from it
*/
bool BlockChain::load( std::string fn ){
	// open the file
	std::ifstream ifs(fn);

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
	Save the BlockChain to the given file path
*/
bool BlockChain::save( std::string fn ){
	// open the file
	std::ofstream ofs(fn);

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
