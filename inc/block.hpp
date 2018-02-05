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

#ifndef _RECHAIN_BLOCK_HPP_
#define _RECHAIN_BLOCK_HPP_

// system includes
#include <inttypes.h>
#include <map>
#include <vector>
#include <string>

// dependency includes
#include "cereal/types/vector.hpp"

/* Maximum hash value (smaller increases difficulty) */
#define HASH_MAX "000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

class Record;

/** A Block to be included in a BlockChain. The Block
	holds any number of Record objects (that represent
	publications or signatures), a hash of the previous
	Block and various hashing variables.
*/
class Block {
	private:
		std::vector<Record> records;	/**< Contained Record objects*/
		std::string prev;		/**< Hash of the previous block */

		long nonce;			/**< Randomly generated value to modify hash */
		long timestamp;			/**< A timestamp */
		uint32_t  counter;		/**< Counter to modify hash output */

	public:
		/** Define a Record iterator */
		typedef std::vector<Record>::iterator iterator;

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

		/** Check if Block is valid
			\returns True if Block is valid
		*/
		bool valid();

		// ------------------------------------------------------
		// Accessor Methods

		/** Overloaded index operator
		    \param i The index to return a reference to
		    \returns A reference to a Record object
		*/
		Record& operator[] ( unsigned int i );
		
		/** Overloaded assignment operator
		    \param b The Block to copy data from
		    \returns A reference to the current Block
		*/
		Block& operator=( const Block& b );
 
		/** Get a Record block given the signature
		    \param s The signature of the Record object to return
		    \returns The requested Record block or empty pointer
		*/
		Block::iterator find( std::string s );

		/** Add a Record block
		    \param r The Record to add
		    \returns True if Record was added
		*/
		bool add( Record& r );

		/** Get or set the hash of the previous block
		    \param h The hash to set
		    \returns The current hash
		*/
		std::string previous( std::string h = "" );

		// ------------------------------------------------------
		// Iterator Methods
		
		/** Return an iterator to the start of the Record collection
		    \returns An iterator
		*/
		Block::iterator begin();

		/** Returns an iterator to the end of the Record collection
		    \returns A vector iterator
		*/ 
		Block::iterator end();
		
		// ------------------------------------------------------
		// Utility Methods
		
		/** Get number of Record objects in the Block
		    \returns Size of the Block as size_t
		*/
		size_t size();

		/** Serialize/Unserialize this Block
		    \param ar The archive to serialize to or from
		*/
		template <class Archive>
		void serialize( Archive& ar ){
			ar(
				CEREAL_NVP(records),
				CEREAL_NVP(prev),
				CEREAL_NVP(nonce),
				CEREAL_NVP(timestamp),
				CEREAL_NVP(counter)
			);
		}
};

#endif
