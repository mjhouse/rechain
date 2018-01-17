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
		rl::get().info("Signing and adding '" + s + "' to blockchain");

		Record r(ifs);
		private_key->sign(r);

		if(!blockchain.contains(r.reference())){
			blockchain.add(r);
			blockchain.save();
		} else {
			rl::get().error("Publication already exists");
		}
	}
	else {
		rl::get().error("Couldn't find the file to add: " + s);
	}

}

void Interface::sign( std::string s ){
	BlockChain& blockchain = BlockChain::get_blockchain();

	try {
		rl::get().info("Adding signature for '" + s.substr(0,20) + "...' to blockchain");
		
		auto a = blockchain.address(s);
		Record r(a.second,a.first);
		private_key->sign(r);

		if(blockchain.contains(r.reference())){
			blockchain.add(r);
			blockchain.save();
		} else {
			rl::get().error("Publication doesn't exist");	
		}

	}catch(const std::out_of_range& e){
		rl::get().error("Couldn't find the referenced record");
	}
}

void Interface::check(){
	BlockChain& blockchain = BlockChain::get_blockchain();
	if(!blockchain.valid()){
		rl::get().error("Blockchain is BAD");
	} else {
		rl::get().error("Blockchain is GOOD");
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

	for(unsigned int i = 0; i < blockchain.size(); ++i){
		rl::get("console").info("Block #" + std::to_string(i) + ":");
		for(unsigned int j = 0; j < blockchain[i].size(); ++j){
			auto record = blockchain[i][j];
			std::string msg;
			std::string type = (record.type() == DataType::Publication) ? "Publication" : "Signature";
			float trust = (type == "Publication") ? blockchain.get_publication_trust(record.reference()) : 0.0f;

			if(j == 0 && i == 0)
				msg = "\tRecord: " + record.reference() + " (Genesis)";
			else
				msg = "\tRecord: " + record.reference();

			rl::get("console")
				.info(msg)
				.info("\t\tType: " + type)
				.info("\t\tTrust: " + std::to_string(trust));
		}
	}
}

void Interface::execute(){
	
	cxxopts::Options options("ReChain","The distributed research journal");
	options.add_options()
		("h,help","Display this usage message")	
		("p,publish","Publish a document",cxxopts::value<std::string>(),"<path>")	
		("c,check","Validate the blockchain")	
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
				blockchain.save(path);
			}
			
			try {
				private_key = PrivateKey::load_file(priv);
				public_key = PublicKey::load_file(publ);
			} catch (const std::invalid_argument& e) {
				rl::get().error(e.what());
			}
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
			
			if(result.count("check"))
				this->check();

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
