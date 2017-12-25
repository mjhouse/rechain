/** \file	signature_block.cpp
	\brief	Implements the SignatureBlock class used to sign a reference to
	be stored in the BlockChain.
*/

#include "signature_block.hpp"

// system includes
#include <string>

BlockType SignatureBlock::type(){
	return BlockType::SIGNATURE_BLOCK;
}

/*
	Get a string containing data in the block with
	the exception of the previous hash and the signature.
*/
std::string SignatureBlock::signed_data(){
	return (this->reference + BasicBlock::signed_data());
}

/*
	Get a string containing all data in the block
*/
std::string SignatureBlock::full_data(){
	return (this->signed_data() + BasicBlock::full_data());
}
