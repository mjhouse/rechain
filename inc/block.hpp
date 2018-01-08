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

#ifndef _BLOCK_HPP_
#define _BLOCK_HPP_

// system includes
#include <vector>
#include <map>
#include <string>

/* Maximum hash value (smaller increases difficulty) */
#define HASH_MAX "000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

class Data;

/** A Block to be included in a BlockChain. The Block
	holds any number of Data objects (that represent
	publications or signatures), a hash of the previous
	Block and various hashing variables.
*/
class Block {
	private:
		std::vector<Data> data;		/**< Contained Data objects*/
		std::string prev;		/**< Hash of the previous block */

		long nonce;			/**< Randomly generated value to modify hash */
		long timestamp;			/**< A timestamp */
		unsigned int counter;		/**< Counter to modify hash output */

	public:
		Block() : nonce(0), timestamp(0), counter(0) {}
		~Block();
		
		// ------------------------------------------------------
		// Mining Methods

		/** Get the hash of this block
			\returns The current hash of the block
		*/
		std::string hash();

		/** Hash and update hashing variables until
		    a valid hash is found
		    \returns A valid hash
		*/
		std::string mine();

		// ------------------------------------------------------
		// Accessor Methods

		Data& operator[] ( unsigned int i ){
			return this->data[i];
		}
		
		Block& operator=( const Block& b ){
			this->data	= b.data;
			this->prev	= b.prev;
			this->nonce	= b.nonce;
			this->timestamp	= b.timestamp;
			this->counter	= b.counter;
			return *this;
		}
 
		/** Get a Data block given the index
		    \param s The signature of the Data object to return
		    \returns The requested Data block or empty pointer
		*/
		std::vector<Data>::iterator find( std::string s );

		/** Add a Data block
			\param d The block to add
			\returns True if Data was added
		*/
		bool add( Data& d );

		/** Get or set the hash of the previous block
		    \param h The hash to set
		*/
		std::string previous( std::string h = "" );

		// ------------------------------------------------------
		// Iterator Methods
		
		/** Return an iterator to the start of the Data
		    \returns An iterator
		*/
		std::vector<Data>::iterator begin();

		/** Returns an iterator to the beginning of the Data
		    \param b A reference to the Block
		    \returns A vector iterator
		*/
		std::vector<Data>::iterator begin( Block& b );
		
		/** Returns an iterator to the end of the Data collection
		    \returns A vector iterator
		*/ 
		std::vector<Data>::iterator end();
		
		/** Returns an iterator to the end of the Data collection
		    \param b A reference to the Block
		    \returns A vector iterator
		*/
		std::vector<Data>::iterator end( Block& b );

		// ------------------------------------------------------
		// Utility Methods
		
		/** Get number of Data objects in the Block
		    \returns Size of the Block as size_t
		*/
		size_t size();
};

#endif
