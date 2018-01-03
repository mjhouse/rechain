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
std::map<std::string,float> BlockChain::trust(){
	// Will hold computed trust for public keys
	std::map<std::string,float> key_trust;
	std::map<std::string,std::string> pub_trust;

	// Iterators
	std::vector<std::shared_ptr<Block>>::iterator b_it;
	std::vector<std::shared_ptr<Data>>::iterator d_it;
 
	// Get the owners public key (public key of first block)
	// and insert it with a value of 1
	std::string gen_key = this->blockchain.at(0)->get_data(0)->get_public_key();
	key_trust.insert( std::pair<std::string,float>(gen_key,1.0f) );

	// Iterate over each block and get the data
	for(b_it = this->blockchain.begin(); b_it != this->blockchain.end(); ++b_it){
		std::shared_ptr<Block> block = *b_it;
		std::vector<std::shared_ptr<Data>> data = block->get_data();
		
		// Iterate over each Data object and calculate trust
		for(d_it = data.begin(); d_it != data.end(); ++d_it ){
			std::shared_ptr<Data> d = *d_it; 
			
			switch(d->get_data_type()){
				case DataType::Publication:
					{
						// If this is a publication Data object, then
						// the public key is added to 'key_trust' if
						// it isn't already there
						
						// Get the public key and the signature
						std::string key = d->get_public_key();
						std::string ref = d->get_signature();

						// Try to insert the public key with an initial
						// value of '0.0'
						key_trust.insert( std::make_pair(key,0.0f) );
						
						// Update the publication record linking the signature
						// to the public key
						pub_trust.insert( std::make_pair(ref,key) );
					}
					break;
				case DataType::Signature:
					{
						// If this is a signature Data object, then half
						// of the signers trust goes to the owner of the
						// referenced document

						// Get the public keys of the document publisher and
						// the signer
						std::string ref_key = pub_trust[ d->get_data_ref() ];
						std::string sig_key = d->get_public_key();
					
						// Get the current trust of each
						float sig_trust	= key_trust[ sig_key ];
						float ref_trust = key_trust[ ref_key ];

						// Move half of the signers trust to the 
						// publisher
						sig_trust = sig_trust/2;
						ref_trust += sig_trust;
					
						// Update the key_trust map
						key_trust[ref_key] = ref_trust;	
						key_trust[sig_key] = sig_trust;	
					}
					break;
			}
		}
	}
	return key_trust;
}	

/* Returns the number of Block objects
*/
size_t BlockChain::size(){
	return this->blockchain.size();
}
