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
BlockChain::BlockChain() : current(nullptr) {}

/* Empty destructor
*/
BlockChain::~BlockChain(){
	for(auto b : this->blockchain){ delete b; }
	this->blockchain.clear();

	delete this->current;
	this->current = nullptr;
}

/* Create a new Block
*/
BlockChain* BlockChain::open_block(){
	if(!this->current) this->current = new Block();
	return this;
}

/* Add Data to an open Block
*/
BlockChain* BlockChain::with_data( Data* d ){
	if(this->current){
		this->current->add_data(d);
	}
	return this;
}

/* Update trust maps
*/
void BlockChain::update_trust(){

	this->usr_trust.clear();
	this->pub_trust.clear();

	std::map<std::string,std::string> ref;

	if(!this->blockchain.empty()){
		std::string gen_key = this->blockchain.at(0)->get_data(0)->get_public_key();
		this->usr_trust.insert( std::make_pair(gen_key,1.0f) );

		for(auto b : this->blockchain){
			for(auto d : b->get_data()){
				switch( d->get_data_type() ){
					case DataType::Publication:

						// init a trust entry for the publisher
						this->usr_trust.insert( std::make_pair( d->get_public_key(), 0.0f ) );
						
						// update the signature -> user reference
						ref.insert( std::make_pair( d->get_signature(), d->get_public_key() ) );	
						
						break;
					case DataType::Signature:
						{
							// Get publication reference, the signer and signee
							// public keys
							std::string pubref = d->get_data_ref();
							std::string signer = d->get_public_key();
							std::string signee = ref[pubref];

							float ct = this->usr_trust[signer]/2.0f;// index will return 0.0f by default
							if(ct && !signee.empty()){
								this->usr_trust[signer] = ct;	// signer loses half of trust
								this->usr_trust[signee] += ct;	// signee gains half of signers trust
								this->pub_trust[pubref] += ct;	// pubref gains half of signers trust
							}
						}
						break;
				}
			}
		}
	}
}

/* Mine and add a block to the chain
*/
std::string BlockChain::mine(){	
	std::string ret_hash;
	if(this->current){
		// Update trust on the block if it's a signature
		this->current->set_trust( this->get_publication_trust() );

		// Check if the chain has a genesis block
		if(this->blockchain.size() > 0){
			this->current->set_previous(this->blockchain.back()->hash());
		}

		// Mine the block until it has a valid hash
		while(this->current->hash() > HASH_MAX){
			
			// Update the hashing variables
			this->current->change_hash();

		}

		// Return the hash
		ret_hash = this->current->hash();

		// Add to the chain
		this->blockchain.push_back( this->current );
		this->current = nullptr;
		
		// Update trust maps
		this->update_trust();
	}
	return ret_hash;
}

/* Get the trust for a publication
*/
float BlockChain::get_publication_trust( std::string s ){
	return this->pub_trust[s];	
}

/* Get the trust for all publications
*/
std::map<std::string,float> BlockChain::get_publication_trust(){
	return this->pub_trust;	
}

/* Get the trust for a public key
*/
float BlockChain::get_user_trust( std::string p ){
	return this->usr_trust[p];
}

/* Get the trust for all public keys
*/
std::map<std::string,float> BlockChain::get_user_trust(){
	return this->usr_trust;
}

/* Iterator begin
*/
std::vector<Block*>::iterator BlockChain::begin(){
	return this->blockchain.begin();
}

/* Iterator begin with reference
*/
std::vector<Block*>::iterator BlockChain::begin( BlockChain& b ){
	return b.blockchain.begin();
}

/* Iterator end
*/
std::vector<Block*>::iterator BlockChain::end(){
	return this->blockchain.end();
}

/* Iterator end with reference
*/
std::vector<Block*>::iterator BlockChain::end( BlockChain& b ){
	return b.blockchain.end();
}


/* Returns the number of Block objects
*/
size_t BlockChain::size(){
	return this->blockchain.size();
}
