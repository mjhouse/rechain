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

// local includes
#include "interface.hpp"
#include "manager.hpp"
#include "blockchain.hpp"
#include "logger.hpp"

#define H_NOERR	0
#define H_ERROR	1

int Interface::execute(){

	cxxopts::Options options("ReChain","The distributed research journal");
	options.add_options()
		("h,help","Display this usage message")	
		("v,version","Display version information")
		("p,publish","Publish a document",cxxopts::value<std::string>(),"<path>")	
		("c,check","Validate the blockchain")	
		("s,sign","Sign a published document",cxxopts::value<std::string>(),"<path>")
		("private_key","Make a private key active",cxxopts::value<std::string>(),"<path>")
		("l,list","List published documents")
		("verbose","All logging output")
		("silent","No logging output");

	
    try {
		
		auto result = options.parse(this->argc,this->argv);

        Level level;
        if(result.count("verbose"))     level = Level::error;
        else if(result.count("silent")) level = Level::none;
        else                            level = Level::info;

        manager = std::shared_ptr<Manager>(new Manager());
        if(!manager->configure(level)){
            return H_ERROR;
        }


        // Check for help first
		if(result.count("help")){
			// Display help for command line options
			std::cout << options.help() << std::endl;
            return H_NOERR;
		}
		else {

            if(result.count("private_key")){
                if(manager->key(result["private_key"].as<std::string>())){
                	return H_NOERR;
                }
				else {
					return H_ERROR;
                }
            }

			// Publish a document to the blockchain
			if(result.count("publish")){
				if(this->manager->publish(result["p"].as<std::string>())){
                	return H_NOERR;
                }
				else {
					return H_ERROR;
                }
			}
			
			// Check blockchain is valid
			if(result.count("check")){
                if(manager->is_valid())
					return H_NOERR;
				else
					return H_ERROR;
			}
			
			// Sign a previously published Record
			if(result.count("sign")){
				if(manager->sign(result["s"].as<std::string>()))
					return H_NOERR;
				else
					return H_ERROR;
				
			}

			// Display the current version
			if(result.count("version")){
                std::cout << "ReChain v" << RECHAIN_VERSION << " (GNU GPL v3) "
					  << "The distributed research journal" << std::endl;
				return H_NOERR;
			}
		}
	} catch (const cxxopts::OptionException& e){
		RCERROR("Failed to parse command line arguments!");
        return H_ERROR;
	}

	return H_NOERR;
}
