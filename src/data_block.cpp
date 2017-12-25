/** \file	data_block.cpp
	\brief	Implements the DataBlock class used to manage
			data stored in the BlockChain.
*/

#include "data_block.hpp"

// system includes
#include <string>

BlockType DataBlock::type(){
	return BlockType::DATA_BLOCK;
}

/*
	Get a string containing data in the block with
	the exception of the previous hash and the signature.
*/
std::string DataBlock::signed_data(){
	return (this->message + BasicBlock::signed_data());
}

/*
	Get a string containing all data in the block
*/
std::string DataBlock::full_data(){
	return (this->signed_data() + BasicBlock::full_data());
}
