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

// dependency includes
#include "spdlog/spdlog.h"

namespace spd = spdlog;

class Interface {
	private:
		int argc;
		char** argv;

		std::shared_ptr<spd::logger> log;	/**< A handle to the logger */
		std::string home;			/**< The path to the home directory */
		
		int help();
		
		int blockchain();
		
		int publish( std::string s );
		
		int sign( std::string s );
		
		int list();


		inline std::string env(std::string const& key){
		    char const* val = std::getenv(key.c_str()); 
		    return (val == NULL) ? std::string() : std::string(val);
		}

		inline std::string trim( std::string s ){
			auto it = s.end() - 1;
			if(*it == '/') s.erase(it);
			return s;
		}

	public:
		Interface( int c, char** v ) : argc(c), argv(v) {
			this->home = env("RECHAIN_HOME");

			if(!this->home.empty()){

				// Create a logger that outputs to a file and stdout (in color)
				std::vector<spd::sink_ptr> sinks;
				sinks.push_back(std::make_shared<spd::sinks::simple_file_sink_st>( (this->home + "/rechain.log") ));

				#if defined(_WIN32) || defined(_WIN64)
					sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
				#elif defined(__linux__)
					sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
				#endif

				// Register the logger
				this->log = std::make_shared<spd::logger>("log", begin(sinks), end(sinks));
				spd::register_logger(this->log);
				
				// Set the log output level
				spd::set_level(spd::level::debug);
			}		

		};
		~Interface(){};

		int execute();
};

#endif
