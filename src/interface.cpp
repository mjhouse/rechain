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
#include "logger.hpp"

#define NOERR	0
#define ERROR	1

typedef Logger rl;

bool Interface::publish( std::string s ){
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
			return false;
		}
	}
	else {
		rl::get().error("Couldn't find the file to add: " + s);
		return false;
	}
	return true;
}

bool Interface::sign( std::string s ){
	rl::get().info("Adding signature for '" + s.substr(0,20) + "...' to blockchain");

	Record record;
	for(auto b : blockchain){
		for(auto r : b){
			if(r.reference() == s){
				record.reference(r.reference());
				record.block(b.hash());
				private_key->sign(record);
			}
		}
	}

	if(record.valid()){
		blockchain.add(record);
		blockchain.save();
	} else {
		rl::get().error("Referenced publication doesn't exist");
		return false;
	}

	return true;
}

void Interface::check(){
	if(!blockchain.valid()){
		rl::get().error("Blockchain is BAD");
	} else {
		rl::get().error("Blockchain is GOOD");
	}
}

bool Interface::mine(){
	rl::get()
		.debug("Mining: ")
		.debug("Hash: " + blockchain.mine());

	if(blockchain.valid() && blockchain.save())
		return true;
	return false;
}

void Interface::list(){
	rl::get("console").info(" ---- Blockchain ---- ");

	for(unsigned int i = 0; i < blockchain.size(); ++i){
		rl::get("console").info("Block #" + std::to_string(i) + ":");
		for(unsigned int j = 0; j < blockchain[i].size(); ++j){
			auto record = blockchain[i][j];
			std::string msg;
			std::string ref = record.reference();
			std::string own = record.public_key();
			std::string type = (record.type() == DataType::Publication) ? "Publication" : "Signature";
			float trust = (type == "Publication") ? blockchain.trust(record.reference()) : 0.0f;

			if(j == 0 && i == 0)
				msg = "\tRecord: " + ref + " (Genesis)";
			else
				msg = "\tRecord: " + ref;

			rl::get("console")
				.info(msg)
				.info("\tOwner:  " + own.substr(50,ref.length()-3) + "...")
				.info("\t\tType:  " + type)
				.info("\t\tTrust: " + std::to_string(trust));
		}
	}
}

int Interface::execute(){

	cxxopts::Options options("ReChain","The distributed research journal");
	options.add_options()
		("h,help","Display this usage message")	
		("v,version","Display version information")
		("p,publish","Publish a document",cxxopts::value<std::string>(),"<path>")	
		("c,check","Validate the blockchain")	
		("m,mine","Mine a block")	
		("s,sign","Sign a published document",cxxopts::value<std::string>(),"<path>")
		("private_key","Make a private key active",cxxopts::value<std::string>(),"<path>")
		("public_key","Make a public key active",cxxopts::value<std::string>(),"<path>")
		("l,list","List published documents")
		("verbose","All logging output")
		("silent","No logging output");

	
    try {
		
		auto result = options.parse(this->argc,this->argv);

        Level level;
        if(result.count("verbose"))     level = Level::error;
        else if(result.count("silent")) level = Level::none;
        else                            level = Level::info;
		
        Logger::get()
            .with( Log("console",STDOUT,level) )
            .with( Log("log",home + "/rechain.log",Level::error) );

        // Check RECHAIN_HOME
        if(!home.empty()){
			std::string path = home + "/rechain.blockchain";
			if(!blockchain.load(path)){
				blockchain.save(path);
			}
		}
		else {
            // or error out
			rl::get().error("RECHAIN_HOME isn't set!");
			return ERROR;
		}

        // Check for help first
		if(result.count("help")){
			// Display help for command line options
			std::cout << options.help() << std::endl;
            return NOERR;
		}
		else {

            // Do other options
			std::string publ = home + "/current.public";
			std::string priv = home + "/current.private";
	
			// Try to load a private key from a given path
			// or from the default path
			try {
				if(result.count("private_key")){
					private_key.reset(PrivateKey::load_file(result["private_key"].as<std::string>()));
					private_key->save(priv);
				} else {
					private_key.reset(PrivateKey::load_file(priv));
				}
			} catch (const std::invalid_argument& e){
				rl::get().error(e.what());
				return ERROR;
			}
		
			// Try to load a public key from a given path
			// or from the default path
			try {
				if(result.count("public_key")){
						public_key.reset(PublicKey::load_file(result["public_key"].as<std::string>()));
						public_key->save(publ);
				} else {
						public_key.reset(PublicKey::load_file(publ));
				}
			} catch (const std::invalid_argument& e){
				rl::get().error(e.what());
				return ERROR;
			}

			// Publish a document to the BlockChain
			if(result.count("publish")){
				if(this->publish(result["p"].as<std::string>()))
					return NOERR;
				else
					return ERROR;
			}
			
			// Check the BlockChain integrity
			if(result.count("check")){
				this->check();
				return NOERR;
			}
			
			// Mine the current Block
			if(result.count("mine")){
				if(this->mine())
					return NOERR;
				else
					return ERROR;
			}

			// Sign a previously published Record
			if(result.count("sign")){
				if(this->sign(result["s"].as<std::string>()))
					return NOERR;
				else
					return ERROR;
				
			}

			// Display the current BlockChain state
			if(result.count("list")){
				this->list();
				return NOERR;
			}

			// Display the current version
			if(result.count("version")){
				std::cout << "ReChain v" << RECHAIN_VERSION << " (GNU GPL v3) "
					  << "The distributed research journal" << std::endl;
				return NOERR;
			}
		}
	} catch (const cxxopts::OptionException& e){
		rl::get().error("Failed to parse command line arguments!");
		return ERROR;
	}

	return NOERR;
}
