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
#include <set>

// dependency includes
#include <cereal/archives/json.hpp>
#include "cereal/types/vector.hpp"

// local includes
#include "blockchain.hpp"
#include "block.hpp"
#include "logger.hpp"

typedef Logger rl;

/* Add a Record to an open Block
*/
BlockChain& BlockChain::add( Record& r ){
	this->current.add(r);
	return *this;
}

/* Update trust maps
*/
void BlockChain::update_trust(){

	if(!this->blockchain.empty() && this->blockchain[0].size() > 0){

		std::map<std::string,std::string> references;
		std::vector<Record> records;

        pub_trust.clear();
        usr_trust.clear();

		int count = 1;
		for(auto b : blockchain){
		    for(auto r : b){
			switch(r.type()){
			    case DataType::Publication:
				references.insert( std::make_pair(r.reference(),r.public_key()) );
			    break;
			    case DataType::Signature:
				records.push_back(r);
				count++; 
			    break;
			}
		    }
		}

		float gt = (float)(count*2);
		std::string owner = blockchain[0][0].public_key();
		usr_trust.insert( std::make_pair(owner,gt) );

		for(auto r : records){
			std::string pubref = r.reference();
			std::string signer = r.public_key();
			std::string signee = references[pubref];
		    
			float ct = usr_trust[signer]/2.0f;
			if(ct && !signee.empty()){
			    usr_trust[signer]	 = ct;	// signer loses half of trust
			    usr_trust[signee]	+= ct;	// signee gains half of signers trust
			    pub_trust[pubref]	+= ct;	// pubref gains half of signers trust
			}
		}
	}
}

/* Mine and add a block to the chain
*/
std::string BlockChain::mine(){	
	// Check if the chain has a genesis block
	if(this->blockchain.size() > 0){
		this->current.previous(this->blockchain.back().hash());
	}

	// Get the hash to return
	std::string hash = this->current.mine();
	
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
	this->pub_trust	 = b.pub_trust;
	return *this;
}

/**
*/
BlockChain::iterator BlockChain::find( std::string h ){
	return std::find_if(blockchain.begin(),blockchain.end(),
	[&h](Block& b){ return (b.hash() == h); });
}

/* Check if a Record already exists
*/
bool BlockChain::contains( std::string s, Search type ){

    switch(type){
        case Search::RecordType:
        {
            // Check in the blockchain
            for(auto b : blockchain)
                for(auto r : b)
                    if(r.reference() == s) return true;
            
            for(auto r : current)
                if(r.reference() == s) return true;
            
            break;
        }
        case Search::BlockType:
        {
            // Check in the blockchain
            for(auto b : blockchain)
                if(b.hash() == s) return true;
            
            break;
        }
    }

    // Can't find it
	return false;
}

/* Check if BlockChain is valid
*/
bool BlockChain::valid(){
	std::string previous;
	std::set<std::string> pubs;

	for(auto b : blockchain){

		if(b.previous() != previous)
			return false;
		if(b.hash() > HASH_MAX)
			return false;

		previous = b.hash();

		for(auto r : b){
			if(!r.valid()) return false;
			
			switch(r.type()){
				case DataType::Publication:
					if(!pubs.insert(r.reference()).second)
						return false;
					break;

				case DataType::Signature:
				{
					// Try to find the referenced block by hash
					auto b_it = this->find(r.block());
					if(b_it == this->end())
						return false;

					// Try to find the referenced publication
					Block& b = *b_it;
					auto r_it = b.find(r.reference());
					if(r_it == b.end()) 
						return false;
				}
				break;
			}
		}
	}
	return true;
}

/* Get the trust for a publication
*/
float BlockChain::trust( std::string r ){
	return this->pub_trust[r];
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

			rl::get().debug("Blockchain saved: " + path);
			return true;
		}
	} 

	
	rl::get().warning("Blockchain failed to save: " + path);	
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

			rl::get().debug("Blockchain loaded: " + path);
			return true;
		}
	}
	
	rl::get().warning("Blockchain failed to load: " + path);	
	return false;
}
