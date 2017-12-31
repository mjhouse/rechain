
// System includes
#include <future>

// Local includes
#include "blockchain.hpp"
#include "block.hpp"
		
/* Empty constructor
*/
BlockChain::BlockChain() : current_process() {}

/* Empty destructor
*/
BlockChain::~BlockChain(){}

/* Mine and add a block to the chain
*/
std::string BlockChain::add_block( std::shared_ptr<Block> block ){	
	
	// Check if the chain has a genesis block
	if(this->blockchain.size() > 0){
		// If it does, set previous hash to the last valid block
		block->set_previous(this->blockchain.back()->hash());
	} else {
		// If it doesn't, set previous empty
		block->set_previous("");
	}

	// Mine the block until it has a valid hash
	while(this->running){
		
		// Update the hashing variables
		block->change_hash();

		// Check if the hash is valid
		if(block->hash() <= HASH_MAX){

			// If so, add to the chain and break
			this->blockchain.push_back( block );
			break;
		}
	}

	// Return the hash
	return block->hash();
}

/* Start a mining operation
*/
bool BlockChain::start(){
	if(this->current_block && !this->running){
		this->running = true;
		this->current_process = std::async(&BlockChain::add_block,this,this->current_block);
		return true;
	}
	return false;
}

/* Stop the current mining operation
*/
void BlockChain::stop(){
	if(this->running){
		this->running = false;
		this->current_process.get();
	}	
}

/* Mine and add a block to the chain
*/
void BlockChain::set_block( std::shared_ptr<Block> block ){
	this->current_block = block;
}

/* Discard all blocks following and including
    the given hash
*/
size_t BlockChain::discard( std::string hash ){
	return 0;
}

/* Discard all blocks following and including
    the given index
*/
size_t BlockChain::discard( unsigned int idx ){
	return 0;
}

/* Calculate the trust for a Block in the chain
*/
size_t BlockChain::trust( std::string hash ){
	return 0;
}	

/* Calculate the trust for the current chain
*/
size_t BlockChain::trust(){
	return 0;
}	

