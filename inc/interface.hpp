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

/** 	\file	interface.hpp
	\brief 	Defines the command line interface
		for ReChain. 
*/

#ifndef _INTERFACE_HPP_
#define _INTERFACE_HPP_

// system includes
#include <string>
#include <iostream>

// local includes
#include "logger.hpp"
#include "keys.hpp"

class Interface {
	private:
		int argc;
		char** argv;

		std::string home;			/**< The path to the home directory */
	
		PrivateKey* private_key;
		PublicKey* public_key;

		void publish( std::string s );
		
		void sign( std::string s );
	
		void check();
		
		void mine();

		void list();

		inline std::string trim( std::string s ){
			auto it = s.end() - 1;
			if(*it == '/') s.erase(it);
			return s;
		}

		inline std::string env(std::string const& key){
		    char const* val = std::getenv(key.c_str()); 
		    return (val == NULL) ? std::string() : trim(std::string(val));
		}

	public:
		Interface( int c, char** v ) : argc(c), argv(v) {
			home = env("RECHAIN_HOME");
			
			Logger::get()
				.with( Log("console",STDOUT,Level::error) )
				.with( Log("log",home + "/rechain.log",Level::error) );

		};

		~Interface(){
			delete private_key;
			delete public_key;
		};

		void execute();
};

#endif
