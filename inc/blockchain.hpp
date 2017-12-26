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
#include <map>					// For std::map

class Block;	/**< Forward declaration of Block */
class KeyPair;	/**< Forward declaration of KeyPair */

class BlockChain {
	private:
		/** The vector of Block objects making up the blockchain */
		std::vector<std::shared_ptr<Block>> chain;

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

		/** Return the number of Block objectss in the chain.
			\returns The size of the BlockChain
		*/
		unsigned int size();

		/** Publish a new block by finding an appropriate hash
			\param new_block Pointer to the signed block to publish
			\returns True when the block is published, false if it isn't signed.
		*/
		bool publish( std::shared_ptr<Block> new_block );

		/** Get a block from the BlockChain
			\param i The index of the block to get
			\returns A pointer to the Block
		*/
		std::shared_ptr<Block> get( int i );

		/** Get a block from the BlockChain
			\param h The hash of the block to get
			\returns A pointer to the Block
		*/
		std::shared_ptr<Block> get( std::string h );

		/** Get the trust assigned to each public key
			\returns A map of public keys to trust score
		*/
		std::map<std::string,int> get_author_trust();

		/** Get the last block from the BlockChain
			\returns A pointer to the Block
		*/
		std::shared_ptr<Block> back();

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

		// ---------------------------------------------------------------------
		// Creation Functions
		/** Create a new genesis block with a public key
			\param key A KeyPair to sign the genesis block with
			\returns A shared_ptr to the genesis block
		*/
		std::shared_ptr<Block> create_genesis_block( KeyPair key );

		/** Create a new data block with a public key and data
			\param data The message to fill the block with
			\param key A KeyPair to sign the data block with
			\returns A shared_ptr to the data block
		*/
		std::shared_ptr<Block> create_data_block( std::string data, KeyPair key );

		/** Create a new signature block with a public key and reference
			\param ref The reference to set the block to
			\param key A KeyPair to sign the reference block with
			\returns A shared_ptr to the reference block
		*/
		std::shared_ptr<Block> create_signature_block( std::shared_ptr<Block> ref, KeyPair key );

		/** Serialization function for cereal library
			\param archive The archive to save the chain to
		*/
		template<class Archive>
		void serialize(Archive& archive) {
			archive(CEREAL_NVP(this->chain));
		}

};

#endif
