/** \file	blockchain.hpp
	\brief	Defines the BlockChain class used to manage,
			load, save and publish any number of Block objects.
*/

#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include <dependencies/cereal/cereal.hpp>		// For serialization
#include <dependencies/cereal/types/vector.hpp>	// For serializing vectors

#include <string>					// For std::string
#include <vector>					// For std::vector

class BasicBlock;	/**< Forward declaration of BasicBlock */

class BlockChain {
	private:
		/** The vector of Block objects making up the blockchain */
		std::vector<std::shared_ptr<BasicBlock>> chain;

		/** Filename to load or save from or to */
		std::string filename;

	public:
		/** Empty constructor */
		BlockChain(){}

		/** Overloaded constructor to init the chain from a file.
			\param fn The path to the saved BlockChain
		*/
		BlockChain( std::string fn ){ this->load(fn); }

		/** Empty destructor */
		~BlockChain(){}

		/** Publish a new block by finding an appropriate hash
			\param new_block Pointer to the signed block to publish
			\returns True when the block is published, false if it isn't signed.
		*/
		bool publish( std::shared_ptr<BasicBlock> new_block );

		/** Get a block from the BlockChain
			\param i The index of the block to get
			\returns A pointer to the Block
		*/
		std::shared_ptr<BasicBlock> get( unsigned int i );

		/** Get a block from the BlockChain
			\param h The hash of the block to get
			\returns A pointer to the Block
		*/
		std::shared_ptr<BasicBlock> get( std::string h );

		/** Load the BlockChain with data from a saved filename
			\returns True if file was successfully loaded
		*/
		bool load();

		/** Load the BlockChain with data from a file
			\param fn The path to the saved BlockChain
			\returns True if file was successfully loaded
		*/
		bool load( std::string fn );

		/** Save the BlockChain to a saved filename
			\returns True if the save was successful
		*/
		bool save();

		/** Save the BlockChain at the file path
			\param fn The path to save data to
			\returns True if the save was successful
		*/
		bool save( std::string fn );

		/** Serialization function for cereal library
			\param archive The archive to save the chain to
		*/
		template<class Archive>
		void serialize(Archive& archive) {
			archive(CEREAL_NVP(this->chain));
		}

};

#endif
