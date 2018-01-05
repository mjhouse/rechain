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
class Data;

/** The BlockChain class manages a collection of 
    Block objects.
*/
class BlockChain {
	private:
		/** The collection of Block objects */
		std::vector<Block*> blockchain;

		Block* current;

		std::map<std::string,float> usr_trust;
		std::map<std::string,float> pub_trust;
		
		void update_trust();	
	public:
		/** Empty constructor
		*/
		BlockChain();

		/** Empty destructor
		*/
		~BlockChain();

		// ------------------------------------------------------
		// Mining Methods

		/** Create a new Block
		    \returns A pointer to the BlockChain
		*/
		BlockChain* open_block();

		/** Add Data to an open Block
		    \param A pointer to a Data block
		    \returns A pointer to the BlockChain
		*/
		BlockChain* with_data( Data* d );

		/** Mine and add a given block to the BlockChain
		    \param block A pointer to the block to add
		    \returns The valid hash of the new Block
		*/	
		std::string mine();

		// ------------------------------------------------------
		// Trust Methods

		/** Get the trust for a publication
		    \param s The signature of the Data object
		    \returns The trust for the Data object
		*/
		float get_publication_trust( std::string s );
		
		/** Get all publication trust as a map 
		    \returns The trust for all publications
		*/
		std::map<std::string,float> get_publication_trust();

		/** Get the trust for a user
		    \param p The public key for the user
		    \returns The trust for the public key
		*/
		float get_user_trust( std::string p );

		/** Get all user trust as a map 
		    \returns The trust for all public keys
		*/
		std::map<std::string,float> get_user_trust();

		// ------------------------------------------------------
		// Iterator Methods
		
		/** Return an iterator to the start of the BlockChain
		    \returns An iterator
		*/
		std::vector<Block*>::iterator begin();

		/** Returns an iterator to the beginning of the BlockChain
		    \param b A reference to the BlockChain
		    \returns A vector iterator
		*/
		std::vector<Block*>::iterator begin( BlockChain& b );
		
		/** Returns an iterator to the end of the BlockChain
		   \returns A vector iterator
		*/ 
		std::vector<Block*>::iterator end();
		
		/** Returns an iterator to the end of the BlockChain
		    \param b A reference to the BlockChain
		    \returns A vector iterator
		*/
		std::vector<Block*>::iterator end( BlockChain& b );

		// ------------------------------------------------------
		// Utility Methods

		/** Return the number of Block objects in the chain
		    \returns The number of Block objects 
		*/
		size_t size();	
};

#endif
