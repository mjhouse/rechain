/** \file	data_block.hpp
	\brief	Defines the DataBlock class that contains data to be stored
			in the BlockChain.
*/

#ifndef DATA_BLOCK_HPP
#define DATA_BLOCK_HPP

#include <dependencies/cereal/types/base_class.hpp>
#include <dependencies/cereal/cereal.hpp>
#include "basic_block.hpp"

/** \brief	Manages, hashes and verifies a block of data
			that is stored in the BlockChain.
*/
class DataBlock : public BasicBlock {

	private:
		// data
		std::string message;	/**< Placeholder data */

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
		DataBlock(){}

		/** Empty destructor
		*/
		~DataBlock(){}

		/** Get the block type
			\returns A BlockType
		*/
		BlockType type();

		/** Get the message
			\returns the block message
		*/
		std::string get_message(){ return this->message; }

		/** Set the message
			\param m the message to set
		*/
		void set_message( std::string m ){ this->message = m; }

		/** Serialize function for the cereal lib.
			\param archive an archive to serialize the block into.
		*/
		template<class Archive>
		void serialize(Archive& archive) {
			archive(
				cereal::base_class<BasicBlock>( this ),
				this->message
			);
		}

};

CEREAL_REGISTER_TYPE(DataBlock)

#endif
