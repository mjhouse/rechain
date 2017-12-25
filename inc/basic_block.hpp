/** \file	basic_block.hpp
	\brief	Defines the BasicBlock class that acts as the parent
	of DataBlock and SignatureBlock.
*/

#ifndef BASIC_BLOCK_HPP
#define BASIC_BLOCK_HPP

/** Enumberated constants for the type of
	block (DataBlock or SignatureBlock).
*/
enum BlockType {
	DATA_BLOCK,		/**< DataBlock type*/
	SIGNATURE_BLOCK	/**< SignatureBlock type*/
};

#include <dependencies/cereal/types/polymorphic.hpp>
#include <dependencies/cereal/cereal.hpp>

/** \brief	Manages, hashes and verifies a block of data
			that is stored in the BlockChain.
*/
class BasicBlock {

	/** So that a KeyPair can sign the BasicBlock */
	friend class KeyPair;

	protected:
		// data
		std::string previous;	/**< The hash of the previous block */
		std::string public_key;	/**< The public key used to sign this block */

		// author information
		std::string signature;	/**< The signature of this block */

		// hashing variables
		long nonce;				/**< Randomly generated value to modify hash */
		long timestamp;			/**< A unix timestamp */
		unsigned int counter;	/**< Counter to modify hash output */

		/** Combine data for signing
			\returns combined data
		*/
		std::string signed_data();

		/** Combine data for hashing
			\returns combined data
		*/
		std::string full_data();

	public:
		/** Empty constructor
		*/
		BasicBlock(): nonce(0), timestamp(0), counter(0) {}

		/** Empty destructor
		*/
		~BasicBlock(){}

		/** Get the block type
			\returns A BlockType
		*/
		virtual BlockType type()=0;

		/** Get a hash of this block
			\returns the full hash of the block
		*/
		std::string get_hash();

		/** Try to publish the block
			\returns true if publish succeeded.
		*/
		bool publish();

		/** Check if the block has correct hash and signature
			\returns true if block is valid
		*/
		bool is_valid();

		/** Check if block is signed correctly
			\returns true if block is signed
		*/
		bool is_signed();

		/** Get the previous blocks hash
			\returns the hash of the previous block
		*/
		std::string get_previous(){ return this->previous; }

		/** Set the hash of the previous block
			\param p the hash to use
		*/
		void set_previous( std::string p ){ this->previous = p; }

		/** Get the public key
			\returns The public key as a hex-encoded string
		*/
		std::string get_public_key(){ return this->public_key; }

		/** Set the public key
			\param p The public key as a hex-encoded string
		*/
		void set_public_key( std::string p ){ this->public_key = p; }

		/** Get the signature for this block
			\returns the signature
		*/
		std::string get_signature(){ return this->signature; }

		/** Set the signature for this block.
			\param s the signature to set
		*/
		void set_signature( std::string s ){ this->signature = s; }

		/** Get the timestamp
			\returns the timestamp
		*/
		long get_timestamp(){ return this->timestamp; }

		/** Serialize function for the cereal lib.
			\param archive an archive to serialize the block into.
		*/
		template<class Archive>
		void serialize(Archive& archive) {
			archive(
				this->previous,
				this->public_key,
				this->signature,
				this->nonce,
				this->timestamp,
				this->counter
			);
		}

};

#endif
