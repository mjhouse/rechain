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
BlockChain::BlockChain() {}

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
	while(block->hash() > HASH_MAX){
		
		// Update the hashing variables
		block->change_hash();

	}

	// Add to the chain
	this->blockchain.push_back( block );
	
	// Return the hash
	return block->hash();
}

/* Discard a block with the given hash
*/
void BlockChain::discard( std::string hash ){
	// Erase Data objects with the given signature
	this->blockchain.erase( std::remove_if(this->blockchain.begin(),this->blockchain.end(),
		[hash]( std::shared_ptr<Block> b ){
			return (b->hash() == hash);
		}
	));
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

