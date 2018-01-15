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

// system includes
#include <vector>
#include <memory>
#include <fstream>
#include <map>

// dependency includes
#include "cereal/types/vector.hpp"
#include "cereal/types/map.hpp"

// local includes
#include "block.hpp"

class Block;
class Data;

/** The BlockChain class manages a collection of 
    Block objects.
*/
class BlockChain {
	private:
		std::vector<Block> blockchain;		/**< Collection of Block objects*/
		std::string file_path;			/**< The path to load or save from/to*/

		Block current;				/**< Current working Block*/

		std::map<std::string,float> usr_trust;	/**< Trust of public keys*/
		std::map<std::string,float> pub_trust;	/**< Trust of publications*/

		/** Empty constructor
		*/
		BlockChain(){};

		/** Update 'usr_trust' and 'pub_trust'
		*/	
		void update_trust();	
	public:
		/** Define a BlockChain iterator */
		typedef std::vector<Block>::iterator iterator;
		
		/** Singleton creation method
			\returns The BlockChain
		*/
		static BlockChain& get_blockchain();

		/** Empty destructor
		*/
		~BlockChain(){};
		
		// ------------------------------------------------------
		// Mining Methods

		/** Add Data to an open Block
			\param d A pointer to a Data block
			\returns A pointer to the BlockChain
		*/
		BlockChain& with( Data d );

		/** Mine the current Block to the BlockChain
			\returns The valid hash of the new Block
		*/	
		std::string mine();

		// ------------------------------------------------------
		// Accessor Methods

		
		/** Overloaded index operator
			\param i The index to return a reference to
			\returns A reference to a Block object
		*/
		Block& operator[] ( unsigned int i );


		/** Overloaded assignment operator
			\param b The BlockChain to copy data from
			\returns A reference to the current BlockChain
		*/
		BlockChain& operator=( const BlockChain& b );
		
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
		BlockChain::iterator begin();

		/** Returns an iterator to the end of the BlockChain
		\returns A vector iterator
		*/ 
		BlockChain::iterator end();

		// ------------------------------------------------------
		// Utility Methods

		/** Return the number of Block objects in the chain
			\returns The number of Block objects 
		*/
		size_t size();	

		/** Serialize/Unserialize this BlockChain
			\param ar The archive to serialize to or from
		*/
		template <class Archive>
		void serialize( Archive& ar ){
			ar(	CEREAL_NVP(blockchain),
				CEREAL_NVP(current),
				CEREAL_NVP(usr_trust),
				CEREAL_NVP(pub_trust)
			);
		}

		/** Save the BlockChain to a given location
			\param p The path to save to
			\returns True if the BlockChain was saved
		*/
		bool save( std::string p = "" );
		
		/** Load the BlockChain from a given location
			\param p The path to load from
			\returns True if the BlockChain was loaded and is valid
		*/
		bool load( std::string p = "" );

};

#endif
