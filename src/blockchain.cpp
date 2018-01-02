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

// System includes
#include <future>

// Local includes
#include "blockchain.hpp"
#include "block.hpp"
		
/* Empty constructor
*/
BlockChain::BlockChain() : current_process() {}

/* Empty destructor
*/
BlockChain::~BlockChain(){}

/* Mine and add a block to the chain
*/
std::string BlockChain::mine( std::shared_ptr<Block> block ){	
	
	// Check if the chain has a genesis block
	if(this->blockchain.size() > 0){
		block->set_previous(this->blockchain.back()->hash());
	}

	// Mine the block until it has a valid hash
	while(this->running){
		
		// Update the hashing variables
		block->change_hash();

		// Check if the hash is valid
		if(block->hash() <= HASH_MAX){

			// If so, add to the chain and break
			this->blockchain.push_back( block );
			break;
		}
	}

	// Return the hash
	return block->hash();
}

/* Discard all blocks following and including
    the given hash
*/
size_t BlockChain::discard( std::string hash ){
	return 0;
}

/* Discard all blocks following and including
    the given index
*/
size_t BlockChain::discard( unsigned int idx ){
	return 0;
}

/* Calculate the trust for a Block in the chain
*/
size_t BlockChain::trust( std::string hash ){
	return 0;
}	

/* Calculate the trust for the current chain
*/
size_t BlockChain::trust(){
	return 0;
}	

