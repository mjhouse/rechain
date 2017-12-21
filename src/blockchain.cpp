#include "blockchain.hpp"
#include "block.hpp"
#include "keys.hpp"

#include <cereal/archives/json.hpp>
#include <fstream>

void BlockChain::publish( Block new_block, KeyPair keys ){
	if(!this->chain.empty())
		new_block.set_previous(this->chain.back().get_hash());
	else
		new_block.set_previous("");

	while(!new_block.publish(keys)){
		std::cout << new_block.get_hash() << std::endl;
	};
	this->chain.push_back(new_block);
}

Block& BlockChain::get( int i ){
	return this->chain[i];
}

void BlockChain::print(){
	std::cout << "-- BLOCKCHAIN --" << std::endl;
	for(unsigned int i = 0; i < this->chain.size(); ++i){
		std::cout << "BLOCK #" << i << std::endl;
		this->chain[i].print();
	}
}

bool BlockChain::load( std::string fn ){
	std::ifstream ifs(fn);
	if(ifs){
		cereal::JSONInputArchive iarchive(ifs);
		iarchive(*this);
		return true;
	}
	return false;
}

bool BlockChain::save( std::string fn ){
	std::ofstream ofs(fn);
	if(ofs){
		cereal::JSONOutputArchive oarchive(ofs);
		oarchive(*this);
		return true;
	}
	return false;
}
