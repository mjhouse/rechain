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

int Interface::execute(){
	
	cxxopts::Options options("ReChain","The distributed research journal");
	options.add_options()
		("h,help","Display this usage message")	
		("b,blockchain","Use a saved blockchain file",cxxopts::value<std::string>(),"<path>")	
		("p,publish","Publish a document",cxxopts::value<std::string>(),"<path>")	
		("s,sign","Sign a published document",cxxopts::value<std::string>(),"<path>")
		("l,list","List published documents");

	BlockChain blockchain;
	std::string blockchain_path;

	try {
		
		auto result = options.parse(this->argc,this->argv);
		
		if(result.count("help")){
			std::cout << options.help() << std::endl;
		} else {
			// paths to search for blockchain file
			std::vector<std::string> paths;

			// if a file is provided, add it first
			if(result.count("blockchain"))
				paths.push_back(result["b"].as<std::string>());


			// Look in the homedir under .rechain
			std::string path = getenv("HOME");
			path.append("/.rechain/rechain.blockchain");

			// Look in the local dir
			paths.push_back(path);
			paths.push_back("rechain.blockchain");

			// Try to load from each path in order- 
			// the one that works is saved
			for(auto p : paths){
				if(blockchain.load(p)) break;
			}

			if(result.count("publish")){
				std::string fpath = result["p"].as<std::string>();
				std::cout << fpath << std::endl;
			}
			
			if(result.count("sign")){
				// I don't know exactly how this will work
			}
			
			if(result.count("list")){
				for(auto block : blockchain){
					for(auto data : block){
						if(data.get_data_type() == DataType::Publication){
							std::cout	<< data.get_data_ref().substr(0,20)
									<< "..." << std::endl;
						}
					}
				}
			}
		}

	} catch (const cxxopts::OptionException& e){
		std::cout << options.help() << std::endl;
		return 1;
	}

	return 0;
}
