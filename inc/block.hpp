#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <cereal/cereal.hpp>

class KeyPair;

// -----------------------------------------------------------------------------
// BLOCK
class Block {
	private:
		// data
		std::string message;
		std::string previous;
		std::string public_key;

		// author information
		std::string signature;	// signature of author on this block
		std::string reference;	// block being referenced

		// hashing variables
		long nonce;
		long timestamp;
		unsigned int counter;

		/*
			if this is a signature block, the message field will be blank,
			and the reference field will be the hash of a different block.
			If this is a publish block, then the message field will NOT
			be blank, and the reference field WILL be.
		*/

		std::string partial_data();
		std::string full_data();
		std::string sign( KeyPair keys );
		bool check_sig();

	public:
		Block(): nonce(0), timestamp(0), counter(0) {}

		~Block(){}

		std::string get_hash();

		bool publish( KeyPair keys );

		bool is_valid();

		std::string get_message(){ return this->message; }
		void set_message( std::string m ){ this->message = m; }

		std::string get_previous(){ return this->previous; }
		void set_previous( std::string p ){ this->previous = p; }

		std::string get_signature(){ return this->signature; }
		void set_signature( std::string s ){ this->signature = s; }

		std::string get_reference(){ return this->reference; }
		void set_reference( std::string r ){ this->reference = r; }

		long get_timestamp(){ return this->timestamp; }

		void print();

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
