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
 * Contact: mjhouse@protonmail.com
 *
*/

// system includes
#include <iostream>
#include <string>
#include <fstream>

// dependency includes
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <cereal/archives/json.hpp>

// local includes
#include "settings.hpp"
#include "utility.hpp"
#include "logger.hpp"

namespace fs = boost::filesystem;
typedef Logger rl;

Settings::Settings(){
	// find the users home directory
    const char* path = std::getenv("RECHAIN_HOME");
	if( path != NULL && (std::strlen(path) != 0)){

        fs::path home(path);

        if(fs::exists(home) && fs::is_directory(home)){
        
            // build paths 
            fs::path config      = home / "rechain.config";
            fs::path log         = home / "rechain.log";
            fs::path public_key  = home / "current.public";
            fs::path private_key = home / "current.private";
            fs::path blockchain  = home / "rechain.blockchain";

            fs::path logs        = home / "logs";
            fs::path files       = home / "files";
            fs::path torrents    = home / "torrents";

            // add the paths to settings
            sets("home",home.string());
            sets("config",config.string());
            sets("public_key",public_key.string());
            sets("private_key",private_key.string());
            sets("log",log.string());
            sets("blockchain",blockchain.string());

            sets("logs",logs.string());
            sets("files",files.string());
            sets("torrents",torrents.string());

            if(fs::exists(config)){
                // load the config file
                this->load(); 
            }
            else {
                // create a new default config file
                this->save();
            }

        }
        else {
            // home directory doesn't exist
        }
	}
	else {
        // RECHAIN_HOME isn't set
    }
}

template <typename T>
T Settings::get( std::string key ){
	std::string result = gets(key);
	return boost::lexical_cast<T>(result);
}

std::string Settings::gets( std::string key ){
    try {
        return this->settings.at(key);
    }
    catch(const std::out_of_range& e){
        rl::get().error("value '" + key + "' isn't in settings!");
        return "";
    }
}

template <typename T>
void Settings::set( std::string key, T value ){
	std::string result = boost::lexical_cast<std::string>(value);
	this->settings[key] = result;
}

void Settings::sets( std::string key, std::string value ){
    this->settings[key] = value;
}

bool Settings::save( std::string path ){
    sets("config",path);
    std::ofstream ofs(path);
    if(ofs.is_open()){

        // serialize settings to the file
        cereal::JSONOutputArchive archive(ofs);
        archive( *this );

        // saved
        return true;
    }

    // couldn't save
    return false;
}

bool Settings::save(){
    return save(gets("config"));
}

bool Settings::load( std::string path ){
    sets("config",path);
    std::ifstream ifs(path);
    if(ifs.is_open()){

        // serialize settings from the file
        cereal::JSONInputArchive archive(ifs);
        archive( *this );

        // loaded
        return true;
    }
    
    // couldn't load
    return false;
}

bool Settings::load(){
    return load(gets("config"));
}

