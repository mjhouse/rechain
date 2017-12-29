#ifndef _BLOCK_HPP_
#define _BLOCK_HPP_

// dependency includes
#include <cryptopp/osrng.h>		// for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type

// system includes
#include <map>
#include <chrono>
#include <string>

class Data;

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
		std::string hash();

		/** Try different hashes until one is found
			that is less than the HASH_MAX
			\returns The matching hash
		*/
		std::string mine();

		/** Get a data block given the signature
			\param s The signature of the block to return
			\returns The requested Data block
		*/
		std::shared_ptr<Data> get_data( std::string s );

		/** Add a Data block
			\param d The block to add
		*/
		bool add_data( std::shared_ptr<Data> d );

		/** Remove a Data block
			\param s The signature of the block to remove
			\return True if found
		*/
		bool remove_data( std::string s );
};

#endif
