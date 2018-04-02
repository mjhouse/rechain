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

#ifndef _RECHAIN_INTERFACE_HPP_
#define _RECHAIN_INTERFACE_HPP_

// system includes
#include <string>
#include <iostream>

// local includes
#include "blockchain.hpp"
#include "keys.hpp"

class Manager;

/** Manages a command line interface to the
	BlockChain and executes arguments 
	against it.
*/
class Interface {
	private:
		int argc;					                	/**< Command line argument count */
		char** argv;			            			/**< Command line arguments */

        std::shared_ptr<Manager> manager;

		/** Trim a trailing slash from a path
			\param s The path to trim
			\returns The trimmed path
		*/
		inline std::string trim( std::string s ){
			auto it = s.end() - 1;
			if(*it == '/') s.erase(it);
			return s;
		}

		/** Get and environment variable
			\param key The key for the environment variable
			\returns The variable as a string or empty on failure
		*/
		inline std::string env(std::string const& key){
		    char const* val = std::getenv(key.c_str()); 
		    return (val == NULL) ? std::string() : trim(std::string(val));
		}

	public:
		/** Constructor
			\param c The count of arguments
			\param v The command line arguments
		*/
		Interface( int c, char** v ) : argc(c), argv(v) {};

		/** Destructor
		*/
		~Interface(){};

        /** Print the current blockchain
        */
        void list();

		/** Execute command line options
			\returns 0 on success, 1 otherwise.
		*/
		int execute();
};

#endif
