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
 * Contact: mjhouse@protonmail.com
 *
*/

// system includes
#include <iostream>
#include <utility>
#include <math.h>
#include <climits>

// dependency includes
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

// local includes
#include "blockchain.hpp"
#include "block.hpp"
#include "logger.hpp"

typedef Logger rl;

#define MAX_TRUST UINT_MAX
#define MIN_TRUST 0

/* Add a Record to an open Block
*/
BlockChain& BlockChain::add( Record& r ){
	this->current.add(r);
	return *this;
}

/* Update trust maps
*/
void BlockChain::update_trust(){

    usr_trust.clear();
    pub_trust.clear();

	if(!this->blockchain.empty() && this->blockchain[0].size() > 0){

		// this map stores references that link author keys to
		// document references.
		std::map<std::string,std::string> reference;

		// the first record in the first block is the owner, they
		// get an initial amount of trust equal to 'MAX_TRUST'.
		std::string owner = blockchain[0][0].public_key();
		usr_trust.insert( std::make_pair(owner,MAX_TRUST) );

		// iterate the through all blocks and all records
		// in each block
		for(auto& block : blockchain){
            // get block author here for transfered-total distribution
            std::string miner = block.public_key();
            unsigned int transferred = 0;

			for(auto& record : block){

				switch(record.type()){
					case DataType::Publication:
						{
							std::string author = record.public_key();
							std::string document = record.reference();

							// add each document hash and author's public key to the reference map
							reference.insert(std::pair<std::string,std::string>(document,author));

                            usr_trust.insert( std::pair<std::string,unsigned int>(author,0) );
						}
						break;
					case DataType::Signature:
						{
							// get the public keys and publication reference
							std::string pubref = record.reference();

							// try to find the signer in the trust map- if they
							// aren't found, then they haven't published anything.
							auto it = usr_trust.find(record.public_key());

							// check if the signer is published
							if( it != usr_trust.end() ){

                                std::string signer = it->first;
                                std::string signee = reference[pubref];

                                unsigned int trust = it->second;

								// get amount to transfer
								unsigned int doc_amount = floor(trust*0.25);
								unsigned int usr_amount = floor(trust*0.25);

								// if they have 1 trust, then doc_amount will be
								// 0, so set doc_amount equal to total trust. we
                                // don't care if users get 0.
                                if(doc_amount == 0)
									doc_amount = trust;

								// transfer trust to the signee
								usr_trust[signee] += usr_amount;
                                pub_trust[pubref] += doc_amount;
								usr_trust[signer] -= (doc_amount + usr_amount);

                                // update the total of transferred trust
                                transferred += doc_amount;
							}
						}
						break;

					default:
						// may add other types of records
						break;
				}
			}

            // update the miner's trust
            usr_trust[miner] += transferred;
		}
	}
}

/* Mine and add a block to the chain
*/
std::string BlockChain::mine( std::string pubkey ){
	// Check if the chain has a genesis block
	if(this->blockchain.size() > 0){
		this->current.previous(this->blockchain.back().hash());
	}

	// Get the hash to return
	std::string hash = this->current.mine(pubkey);

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
