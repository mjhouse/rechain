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

// system includes
#include <iostream>

// dependency includes
#include <cereal/archives/json.hpp>
#include "cereal/types/vector.hpp"

// local includes
#include "blockchain.hpp"
#include "block.hpp"
#include "data.hpp"

typedef Logger rl;

/* Get or create a new BlockChain
*/
BlockChain& BlockChain::get_blockchain(){
	static BlockChain b;
	return b;
}
		
/* Add Data to an open Block
*/
BlockChain& BlockChain::with( Data d ){
	this->current.add(d);
	return *this;
}

/* Update trust maps
*/
void BlockChain::update_trust(){

	this->usr_trust.clear();
	this->pub_trust.clear();

	std::map<std::string,std::string> ref;

	if(!this->blockchain.empty() && this->blockchain[0].size() > 0){
		std::string gen_key = this->blockchain[0][0].get_public_key();
		this->usr_trust.insert( std::make_pair(gen_key,1.0f) );

		for(auto b : this->blockchain){
			for(auto d : b){
				switch( d.get_data_type() ){
					case DataType::Publication:

						// init a trust entry for the publisher
						this->usr_trust.insert( std::make_pair( d.get_public_key(), 0.0f ) );
						
						// update the signature -> user reference
						ref.insert( std::make_pair( d.get_signature(), d.get_public_key() ) );	
						
						break;
					case DataType::Signature:
						{
							// Get publication reference, the signer and signee
							// public keys
							std::string pubref = d.get_data_ref();
							std::string signer = d.get_public_key();
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
	std::string hash; 
	// Update trust on the block if it's a signature
	for(auto d : this->current){
		if(d.get_data_type() == DataType::Signature){
			float t = this->pub_trust[d.get_public_key()];
			d.set_trust( t );
		}
	}

	// Check if the chain has a genesis block
	if(this->blockchain.size() > 0){
		this->current.previous(this->blockchain.back().hash());
	}

	// Get the hash to return
	hash = this->current.mine();
	
	// Add to the chain
	this->blockchain.push_back( this->current );
	this->current = Block();	

	// Update trust maps
	this->update_trust();
	
	return hash;
}

/** Overloaded index operator
*/
Block& BlockChain::operator[] ( unsigned int i ){
	return this->blockchain[i];
}


/** Overloaded assignment operator
*/
BlockChain& BlockChain::operator=( const BlockChain& b ){
	this->blockchain = b.blockchain;
	this->current	 = b.current;
	this->usr_trust	 = b.usr_trust;
	this->pub_trust	 = b.pub_trust;
	return *this;
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
BlockChain::iterator BlockChain::begin(){
	return this->blockchain.begin();
}

/* Iterator end
*/
BlockChain::iterator BlockChain::end(){
	return this->blockchain.end();
}

/* Returns the number of Block objects
*/
size_t BlockChain::size(){
	return this->blockchain.size();
}

/** Save the BlockChain to a given location
*/
bool BlockChain::save( std::string p ){
	std::string path = (!p.empty()) ? p : this->file_path;
	if(!path.empty()){
		std::ofstream os(path);
		if(os.is_open()){
			cereal::JSONOutputArchive archive(os);
			archive( *this );
			this->file_path = path;

			rl::get().debug("Blockchain saved to: " + path);
			return true;
		}
	} 

	
	rl::get().error("Blockchain FAILED to save: " + path);	
	return false;
}

/** Load the BlockChain from a given location
*/
bool BlockChain::load( std::string p ){
	std::string path = (!p.empty()) ? p : this->file_path;
	if(!path.empty()){
		std::ifstream is(path);
		if(is.is_open()){
			cereal::JSONInputArchive archive(is);
			archive( *this );
			this->file_path = path;
			this->update_trust();

			rl::get().debug("Blockchain loaded from: " + path);
			return true;
		}
	}
	
	rl::get().error("Blockchain FAILED to load: " + path);	
	return false;
}
