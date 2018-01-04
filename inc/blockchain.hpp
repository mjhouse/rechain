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

/** 	\file  	blockchain.hpp
	\brief 	Defines the BlockChain class that 
		manages Block objects, searches
		and verifies the Block collection. 
*/

#ifndef _BLOCKCHAIN_HPP_
#define _BLOCKCHAIN_HPP_

#include <vector>
#include <memory>
#include <map>

class Block;

/** The BlockChain class manages a collection of 
    Block objects.
*/
class BlockChain {
	private:
		/** The collection of Block objects */
		std::vector<std::shared_ptr<Block>> blockchain;
		
	public:
		/** Empty constructor
		*/
		BlockChain();

		/** Empty destructor
		*/
		~BlockChain();

		/** Mine and add a given block to the BlockChain
		    \param block A pointer to the block to add
		    \returns The valid hash of the new Block
		*/	
		std::string mine( std::shared_ptr<Block> block );

		/** Get a Block by hash
		    \param hash The hash of the Block to get
		    \returns a pointer to a Block
		*/
		std::shared_ptr<Block> get_block( std::string hash );

		/** Discard a block with the given hash
		    \param hash The hash to discard from
		*/
		void discard( std::string hash );

		/** Calculate the trust for a publication
		    \param s The signature of the Data object
		    \returns The trust for the Data object
		*/
		float get_publication_trust( std::string s );

		/** Calculate the key trust for the current chain
		    \returns The trust score for all public keys
		*/
		std::map<std::string,float> get_trust();
		
		/** Return the number of Block objects in the chain
		    \returns The number of Block objects 
		*/
		size_t size();	
};

#endif
