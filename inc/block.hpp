#ifndef _BLOCK_HPP_
#define _BLOCK_HPP_

#include <cryptopp/osrng.h>		// for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type
#include <cryptopp/hex.h>		// for the HexEncoder

// system includes
#include <iostream>
#include <algorithm>
#include <map>

#include <chrono>
#include <climits>
#include <string>

#include "data.hpp"

/** Maximum hash value (smaller increases difficulty) */
#define HASH_MAX  	"00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

class Block {
	private:
		std::map<std::string,std::shared_ptr<Data>> data;	/**< Contained Data objects*/
		std::string previous;								/**< Hash of the previous block */

		long nonce;											/**< Randomly generated value to modify hash */
		long timestamp;										/**< A timestamp */
		unsigned int counter;								/**< Counter to modify hash output */

		inline long new_nonce(){
			CryptoPP::AutoSeededRandomPool rng;
			return CryptoPP::Integer(rng,CryptoPP::Integer(1),CryptoPP::Integer(LONG_MAX)).ConvertToLong();
		}

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
		std::string hash(){
			std::string hash_data;

			// Get the hashable data for each Data block
			std::for_each( this->data.begin(), this->data.end(),
			[&hash_data]( std::pair<std::string,std::shared_ptr<Data>> el ){
				hash_data.append( el.second->to_string(true) );
			});

			hash_data.append( this->previous );
			hash_data.append( std::to_string(this->nonce) );
			hash_data.append( std::to_string(this->timestamp) );
			hash_data.append( std::to_string(this->counter) );

			// Create a hash
			CryptoPP::SHA256 hasher;
			std::string new_hash;

			// Hash, hex encode and save to new_hash
			CryptoPP::StringSource ss(hash_data,true,
				new CryptoPP::HashFilter(hasher,
					new CryptoPP::HexEncoder(
						new CryptoPP::StringSink(new_hash))));

			// Return new_hash
			return new_hash;
		}

		/** Try different hashes until one is found
			that is less than the HASH_MAX
			\returns The matching hash
		*/
		std::string mine(){

			while(this->hash() > HASH_MAX){

				// Update the counter, and reset to 0 if it
				// gets to the int max
				if(this->counter >= UINT_MAX-1) this->counter = 0;
				else this->counter++;

				// Update the nonce and timestamp
				this->nonce		= new_nonce();
				this->timestamp = new_timestamp();

			}
			// Return the final hash
			return this->hash();
		}

		/** Get a data block given the signature
			\param s The signature of the block to return
			\returns The requested Data block
		*/
		std::shared_ptr<Data> get_data( std::string s ){
			// Create a new shared_ptr and try to
			// get an iterator to the appropriate map
			// pair.
			std::shared_ptr<Data> d;
			auto it = this->data.find(s);

			// If the iterator points to an
			// entry, point the shared_ptr at it.
			if(it != this->data.end())
				d = it->second;

			// Return d
			return d;
		}

		/** Add a Data block
			\param d The block to add
		*/
		bool add_data( std::shared_ptr<Data> d ){
			// Check the Data object is signed and
			// valid.
			if(d->verify()){
				// Try to insert the new Data pointer
				auto result = this->data.insert(std::pair<std::string,std::shared_ptr<Data>>( d->get_signature(), d ));

				// Return the results of the insertion
				return result.second;
			}
			// Return failure if the Data pointer
			// isn't valid.
			return false;
		}

		/** Remove a Data block
			\param s The signature of the block to remove
			\return True if found
		*/
		bool remove_data( std::string s ){
			if(this->data.erase(s) > 0)
				return true;
			return false;
		}

};

#endif
