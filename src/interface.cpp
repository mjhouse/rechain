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
#include "record.hpp"
#include "block.hpp"
#include "blockchain.hpp"

#define NOERR	0
#define ERROR	1

typedef Logger rl;

void Interface::publish( std::string s ){
	BlockChain& blockchain = BlockChain::get_blockchain();

	std::ifstream ifs(s);
	if(ifs.is_open()){
	
		rl::get().debug("Adding data block...");
		blockchain.with( private_key->sign( Record(ifs) ) );

		blockchain.save();
	}

}

void Interface::sign( std::string s ){
	std::ifstream ifs(s);
	if(ifs.is_open()){
	
	}
}

void Interface::mine(){
	BlockChain& blockchain = BlockChain::get_blockchain();

	rl::get()
		.debug("Mining: ")
		.debug("Hash: " + blockchain.mine());

	blockchain.save();
}

void Interface::list(){
	BlockChain& blockchain = BlockChain::get_blockchain();
	rl::get("console").info(" ---- Blockchain ---- ");

	for(auto block : blockchain){
		rl::get("console").info("Block:");
		for(auto r : block){
			rl::get("console")
				.info("\tData: " + r.reference().substr(0,20));
		}
	}
}

void Interface::execute(){
	
	cxxopts::Options options("ReChain","The distributed research journal");
	options.add_options()
		("h,help","Display this usage message")	
		("p,publish","Publish a document",cxxopts::value<std::string>(),"<path>")	
		("m,mine","Mine a block")	
		("s,sign","Sign a published document",cxxopts::value<std::string>(),"<path>")
		("l,list","List published documents");

	try {
		
		auto result = options.parse(this->argc,this->argv);
		
		if(!home.empty()){
			std::string path = home + "/rechain.blockchain";
			std::string priv = home + "/keys/rsa.private";
			std::string publ = home + "/keys/rsa.public";

			BlockChain& blockchain = BlockChain::get_blockchain();

			if(!blockchain.load(path)){
				rl::get().info("couldn't load blockchain from: " + path);
				blockchain.save(path);
			}
			
			private_key = PrivateKey::load_file(priv);
			public_key = PublicKey::load_file(publ);
		}
		else {
			rl::get().error("RECHAIN_HOME isn't set!");
			exit(1);
		}

		if(result.count("help")){
			std::cout << options.help() << std::endl;
		}
		else {
			if(result.count("publish"))
				this->publish(result["p"].as<std::string>());
			
			if(result.count("mine"))
				this->mine();

			if(result.count("sign"))
				this->sign(result["s"].as<std::string>());
			
			if(result.count("list"))
				this->list();
		}
	} catch (const cxxopts::OptionException& e){
		rl::get().error("Failed to parse command line arguments!");
	}
}
