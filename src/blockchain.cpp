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
#include <iostream>

// Local includes
#include "blockchain.hpp"
#include "block.hpp"
#include "data.hpp"
		
/* Empty constructor
*/
BlockChain::BlockChain() {}

/* Empty destructor
*/
BlockChain::~BlockChain(){}

/* Mine and add a block to the chain
*/
std::string BlockChain::mine( std::shared_ptr<Block> block ){	


	// Update trust on the block if it's a signature
	block->set_trust( this->get_trust() );

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

/** Get a Block by hash
*/
std::shared_ptr<Block> BlockChain::get_block( std::string hash ){
	std::shared_ptr<Block> block;
	for(auto b : this->blockchain){
		if(b->hash() == hash){
			block = b;
		}
	}
	return block;
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

/* Calculate the trust for a document
*/
float BlockChain::get_publication_trust( std::string s ){
	float trust = 0;
	for(auto b : this->blockchain){
		for(auto d : b->get_data()){
			if(d->get_data_type() == DataType::Signature && d->get_data_ref() == s ){
				trust += d->get_trust();
			}
		}
	}
	return trust;
}

/* Calculate the trust for the public keys
*/
std::map<std::string,float> BlockChain::get_trust(){
	std::map<std::string,float> trust;
	std::map<std::string,std::string> ref;

	if(!this->blockchain.empty()){
		std::string gen_key = this->blockchain.at(0)->get_data(0)->get_public_key();
		trust.insert( std::make_pair(gen_key,1.0f) );

		for(auto b : this->blockchain){
			for(auto d : b->get_data()){
				switch( d->get_data_type() ){
					case DataType::Publication:
						trust.insert( std::make_pair( d->get_public_key(), 0.0f ) );
						ref.insert( std::make_pair( d->get_signature(), d->get_public_key() ) );	
						break;
					case DataType::Signature:
						try {
							std::string signer = d->get_public_key();
							std::string signee = ref.at(d->get_data_ref());
							float ct = trust.at(signer)/2;
							trust[signer] = ct;
							trust[signee] += ct;
							
						} catch(const std::out_of_range& e){/* continue */}	
						break;
				}
			}
		}
	}
	
	return trust;
}

/* Returns the number of Block objects
*/
size_t BlockChain::size(){
	return this->blockchain.size();
}
