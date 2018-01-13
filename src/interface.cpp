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

// system includes
#include <iostream>
#include <sstream>
#include <string>

// dependency includes
#include "cxxopts.hpp"
#include <cryptopp/osrng.h>	// for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type
#include <cryptopp/hex.h>	// for the HexEncoder

// local includes
#include "interface.hpp"
#include "data.hpp"
#include "block.hpp"
#include "blockchain.hpp"

#define NOERR	0
#define ERROR	1

typedef Logger rl;

int Interface::publish( std::string s ){
	BlockChain& blockchain = BlockChain::get_blockchain();

	std::ifstream ifs(s);
	if(ifs.is_open()){
	
		CryptoPP::SHA256 hasher;
		
		std::string data = [&ifs]{
			std::ostringstream ss{};
			ss << ifs.rdbuf();
			return ss.str(); }();

		std::string new_hash;

		CryptoPP::StringSource ss(data,true,
			new CryptoPP::HashFilter(hasher,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(new_hash)))); 
		
		std::cout << blockchain.new_block()
			.with_data(Data(Address(new_hash,"",DataType::Publication)))
			.mine();

		if(blockchain.save(this->home + "/rechain.blockchain"))
			return NOERR;
	}

	return ERROR;
}

int Interface::sign( std::string s ){
	BlockChain& blockchain = BlockChain::get_blockchain();

	std::ifstream ifs(s);
	if(ifs.is_open()){
	
		return NOERR;
	}

	return ERROR;
}

int Interface::list(){
	BlockChain& blockchain = BlockChain::get_blockchain();
	for(auto block : blockchain){
		rl::get("console").info("BLOCK:");
		for(auto data : block){
			rl::get("console")
				.info("\tData: " + data.get_data_ref().substr(0,20));
		}
	}
	return NOERR;
}

int Interface::execute(){

	BlockChain& blockchain = BlockChain::get_blockchain();
	
	cxxopts::Options options("ReChain","The distributed research journal");
	options.add_options()
		("h,help","Display this usage message")	
		("b,blockchain","Use a saved blockchain file",cxxopts::value<std::string>(),"<path>")	
		("p,publish","Publish a document",cxxopts::value<std::string>(),"<path>")	
		("s,sign","Sign a published document",cxxopts::value<std::string>(),"<path>")
		("l,list","List published documents");

	try {
		
		auto result = options.parse(this->argc,this->argv);
		
		if(result.count("help")){
			std::cout << options.help() << std::endl;
		} else {
			
			if(result.count("blockchain")){
				// Try to load the blockchain from a given path
				std::string path = result["b"].as<std::string>();
				if(!blockchain.load(path)){
					rl::get().info("couldn't load blockchain from: " + path);
				}
			} 
			else if(!home.empty()){
				// Try to load the blockchain from the home dir
				std::string path = home + "/rechain.blockchain";
				if(!blockchain.load(path)){
					rl::get().info("couldn't load blockchain from: " + path);
				}
			}
			else {
				// If RECHAIN_HOME isn't set, exit with error
				rl::get().error("RECHAIN_HOME isn't set!");
				exit(1);
			}

			if(result.count("help")){
				std::cout << options.help() << std::endl;
			}
			
			if(result.count("publish")){
				return this->publish(result["p"].as<std::string>());
			}
			
			if(result.count("sign")){
				return this->sign(result["s"].as<std::string>());
			}
			
			if(result.count("list")){
				return this->list();
			}
		}

	} catch (const cxxopts::OptionException& e){
		// ERROR can't parse args
	}

	return 0;
}
