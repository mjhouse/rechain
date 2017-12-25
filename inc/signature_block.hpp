/** \file	signature_block.hpp
	\brief	Defines the SignatureBlock class that contains a reference signature
	to be stored in the BlockChain.
*/

#ifndef SIGNATURE_BLOCK_HPP
#define SIGNATURE_BLOCK_HPP

#include <dependencies/cereal/types/base_class.hpp>
#include <dependencies/cereal/cereal.hpp>
#include "basic_block.hpp"

/** \brief	Manages, hashes and verifies a block of data
			that is stored in the BlockChain.
*/
class SignatureBlock : public BasicBlock {

	private:
		// data
		std::string reference;	/**< Reference hash */

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
		SignatureBlock(){}

		/** Empty destructor
		*/
		~SignatureBlock(){}

		/** Get the block type
			\returns A BlockType
		*/
		BlockType type();

		/** Get the reference
			\returns the block reference
		*/
		std::string get_reference(){ return this->reference; }

		/** Set the reference
			\param r the reference to set
		*/
		void set_reference( std::string r ){ this->reference = r; }

		/** Serialize function for the cereal lib.
			\param archive an archive to serialize the block into.
		*/
		template<class Archive>
		void serialize(Archive& archive) {
			archive(
				cereal::base_class<BasicBlock>( this ),
				this->reference
			);
		}

};

CEREAL_REGISTER_TYPE(SignatureBlock)

#endif
