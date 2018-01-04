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

// dependency includes
#include <cryptopp/osrng.h>		// for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type

// system includes
#include <vector>
#include <chrono>
#include <string>

/* Maximum hash value (smaller increases difficulty) */
#define HASH_MAX  	"000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

class Data;

/** A Block to be included in a BlockChain. The Block
	holds any number of Data objects (that represent
	publications or signatures), a hash of the previous
	Block and various hashing variables.
*/

class Block {
	private:
		std::vector<std::shared_ptr<Data>> data;		/**< Contained Data objects*/
		std::string previous;					/**< Hash of the previous block */

		long nonce;						/**< Randomly generated value to modify hash */
		long timestamp;						/**< A timestamp */
		unsigned int counter;					/**< Counter to modify hash output */

		/** Generate a new random long between 1 and LONG_MAX
			\returns A random number
		*/
		inline long new_nonce(){
			CryptoPP::AutoSeededRandomPool rng;
			return CryptoPP::Integer(rng,CryptoPP::Integer(1),CryptoPP::Integer(LONG_MAX)).ConvertToLong();
		}

		/** Get a UNIX timestamp
			\returns a timestamp as a long
		*/
		inline long new_timestamp(){
			auto e = std::chrono::system_clock::now().time_since_epoch();
			auto seconds = std::chrono::duration_cast<std::chrono::seconds>(e).count();
			return (long)seconds;
		}

	public:
		Block() : nonce(0), timestamp(0), counter(0) {}
		~Block(){}

		/** Get the hash of this block
			\returns The current hash of the block
		*/
		std::string hash();

		/** Update the hashing variables
		*/
		void change_hash();

		/** Update the trust for signatures
		    \param trust A map of public keys and current trust
		*/
		void set_trust( std::map<std::string,float> trust );
		
		/** Get a Data block given the signature
			\param s The signature of the Data object to return
			\returns The requested Data block or empty pointer
		*/
		std::shared_ptr<Data> get_data( std::string s );

		/** Get a Data block given the index
		    \param i The index of the Data object to return
		    \returns The requested Data block or empty pointer
		*/
		std::shared_ptr<Data> get_data( unsigned int i );
		
		/** Get all Data objects
		    \returns A vector of Data pointers
		*/
		std::vector<std::shared_ptr<Data>> get_data();


		/** Add a Data block
			\param d The block to add
			\returns True if Data was added
		*/
		bool add_data( std::shared_ptr<Data> d );

		/** Remove a Data block
			\param s The signature of the block to remove
			\return True if found
		*/
		void remove_data( std::string s );
		
		/** Get the hash of the previous block
		    \returns The hash of the previous block
		*/
		std::string get_previous();


		/** Set the hash of the previous block
		    \param h The hash to set
		*/
		void set_previous( std::string h );

		/** Get number of Data objects in the Block
			\returns Size of the Block as size_t
		*/
		size_t size();
};

#endif
