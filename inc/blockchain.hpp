/*
 * ReChain: The distributed research journal
 * Copyright (C) 2018  Michael House
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: michaelhouse@gmx.us
 *
*/

/** 	\file  	blockchain.hpp
	\brief 	Defines the BlockChain class that 
		manages Block objects, searches
		and verifies the Block collection. 
*/

#ifndef _BLOCKCHAIN_HPP_
#define _BLOCKCHAIN_HPP_

#include <vector>
#include <memory>
#include <future>

class Block;

/** The BlockChain class manages a collection of 
    Block objects.
*/
class BlockChain {
	public:
		/** The collection of Block objects */
		std::vector<std::shared_ptr<Block>> blockchain;

		/** The Block currently being mined */
		std::shared_ptr<Block> current_block;
		
		/** The current mining process */
		std::future<std::string> current_process;

		/** A flag to indicate whether the 
		    BlockChain is mining or not
		*/
		bool running;		

		/** Mine and add a given block to the BlockChain
		    \param block A pointer to the block to add
		    \returns The valid hash of the new Block
		*/	
		std::string mine( std::shared_ptr<Block> block );

	public:
		/** Empty constructor
		*/
		BlockChain();

		/** Empty destructor
		*/
		~BlockChain();

		/** Start mining the current block
		*/
		void start();

		/** Stop mining the current block
		*/
		void stop();

		/** Wait for mining operation to complete
		*/
		void wait();

		/** Set a Block to be mined
		    \param block A pointer to the block to mine
		*/
		void set_block( std::shared_ptr<Block> block );
	
		/** Discard all blocks following and including
		    the given hash
		    \param hash The hash to discard from
		    \returns The number of blocks discarded
		*/
		size_t discard( std::string hash );
		
		/** Discard all blocks following and including
		    the given index
		    \param idx The index to discard from
		    \returns The number of blocks discarded
		*/
		size_t discard( unsigned int idx );
		
		/** Calculate the trust for a Block in the chain
		    \param hash The hash of the Block
		    \returns The trust score for the Block
		*/
		size_t trust( std::string hash );	
		
		/** Calculate the trust for the current chain
		    \returns The trust score for all documents
		*/
		size_t trust();	
};

#endif