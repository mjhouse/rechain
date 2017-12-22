/** \file	block.hpp
	\brief	Defines the Block class that contains data to be stored
			in the BlockChain.
*/

#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <cereal/cereal.hpp>

/** Forward declaration of KeyPair */
class KeyPair;

/** \brief	Manages, hashes and verifies a block of data
			that is stored in the BlockChain.
*/
class Block {
	private:
		// data
		std::string message;	/**< Placeholder data */
		std::string previous;	/**< The hash of the previous block */
		std::string public_key;	/**< The public key used to sign this block */

		// author information
		std::string signature;	/**< The signature of this block */
		std::string reference;	/**< A reference for signature blocks */

		// hashing variables
		long nonce;				/**< Randomly generated value to modify hash */
		long timestamp;			/**< A unix timestamp */
		unsigned int counter;	/**< Counter to modify hash output */

		/** Combine data for signing
			\returns combined data
		*/
		std::string partial_data();

		/** Combine data for hashing
			\returns combined data
		*/
		std::string full_data();

	public:
		/** Empty constructor
		*/
		Block(): nonce(0), timestamp(0), counter(0) {}

		/** Empty destructor
		*/
		~Block(){}

		/** Get a hash of this block
			\returns the full hash of the block
		*/
		std::string get_hash();

		/** Publish the block, signing with the given keys.
			\param keys the KeyPair to sign the block with.
			\returns true if publish succeeded.
		*/
		bool publish( KeyPair keys );

		/** Check if the block has correct hash and signature
			\returns true if block is valid
		*/
		bool is_valid();

		/** Get the message
			\returns the block message
		*/
		std::string get_message(){ return this->message; }

		/** Set the message
			\param m the message to set
		*/
		void set_message( std::string m ){ this->message = m; }

		/** Get the previous blocks hash
			\returns the hash of the previous block
		*/
		std::string get_previous(){ return this->previous; }

		/** Set the hash of the previous block
			\param p the hash to use
		*/
		void set_previous( std::string p ){ this->previous = p; }

		/** Get the signature for this block
			\returns the signature
		*/
		std::string get_signature(){ return this->signature; }

		/** Set the signature for this block.
			\param s the signature to set
		*/
		void set_signature( std::string s ){ this->signature = s; }

		/** Get the referenced block
			\returns the hash of the referenced block
		*/
		std::string get_reference(){ return this->reference; }

		/** Set the referenced block
			\param r the hash of the referenced block
		*/
		void set_reference( std::string r ){ this->reference = r; }

		/** Get the timestamp
			\returns the timestamp
		*/
		long get_timestamp(){ return this->timestamp; }

		/** Print data for debugging */
		void print();

		/** Serialize function for the cereal lib.
			\param archive an archive to serialize the block into.
		*/
		template<class Archive>
		void serialize(Archive& archive) {
			archive(
				this->message,
				this->previous,
				this->public_key,
				this->signature,
				this->reference,
				this->nonce,
				this->timestamp,
				this->counter
			);
		}

};

#endif
