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
#include <string>

// dependency includes
#include "cxxopts.hpp"

// local includes
#include "interface.hpp"
#include "data.hpp"
#include "block.hpp"
#include "blockchain.hpp"

#define trim_slash(X)(\
)


int Interface::help(){}

int Interface::blockchain(){}

int Interface::publish( std::string s ){}

int Interface::sign( std::string s ){}

int Interface::list(){}

int Interface::execute(){

	std::shared_ptr<BlockChain> blockchain(BlockChain::get_blockchain());
	
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
				if(!blockchain->load(result["b"].as<std::string>())){
					this->log->error("Couldn't load blockchain from {}",
						result["b"].as<std::string>());
				}
			} 
			else if(!this->home.empty()){
				std::string bpath = trim(this->home) + "/rechain.blockchain";
				if(!blockchain->load(bpath)){
					this->log->error("Couldn't load blockchain from {}",bpath);
				}
			}
			else {
				this->log->error("Can't find a blockchain file!");
			}

			if(result.count("help")){
				return this->help();
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
