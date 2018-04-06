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

namespace fs = boost::filesystem;

Settings::Settings(){}

Settings* Settings::instance(){
    static Settings settings;
    return &settings;
}

bool Settings::initialize(){
	// find the users home directory
    const char* path = std::getenv("RECHAIN_HOME");
	if( path != NULL && (std::strlen(path) != 0)){

        fs::path home(path);

        if(fs::exists(home) && fs::is_directory(home)){
        
            // build a path to the expected config location
            fs::path config      = home / "rechain.config";
            fs::path log         = home / "rechain.log";
            fs::path public_key  = home / "current.public";
            fs::path private_key = home / "current.private";

            // add the home path and config file path to settings
            sets("home",home.string());
            sets("config",config.string());
            sets("public_key",public_key.string());
            sets("private_key",private_key.string());
            sets("log",log.string());

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
            return false;
        }
	}
	else {
        // RECHAIN_HOME isn't set
        return false;
    }

    // settings initialized
    initialized = true;
    return true;
}

template <typename T>
T Settings::get( std::string key ){
	std::string result = this->settings.at(key);
	return boost::lexical_cast<T>(result);
}

std::string Settings::gets( std::string key ){
    return this->settings.at(key);
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

