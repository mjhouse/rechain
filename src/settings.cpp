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
#include <stdexcept>
#include <string>

// dependency includes
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <cereal/archives/json.hpp>

// local includes
#include "settings.hpp"
#include "logger.hpp"
#include "utility.hpp"

namespace fs = boost::filesystem;
typedef Logger rl;

Settings::Settings(){

	// find the users home directory
	const char *path;
	fs::path home;
	if(( path = getenv("RECHAIN_HOME")) != NULL){

		home = fs::path(path);

        if(fs::exists(home) && fs::is_directory(home)){
        
            // build a path to the expected config location
            fs::path config      = home / "rechain.config";
            fs::path log         = home / "rechain.log";
            fs::path public_key  = home / "current.public";
            fs::path private_key = home / "current.private";

            // add the home path and config file path to settings
            set<std::string>("home",home.string());
            set<std::string>("config",config.string());
            set<std::string>("public_key",public_key.string());
            set<std::string>("private_key",private_key.string());
            set<std::string>("log",log.string());

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
            // throw an error: home directory doesn't exist
            rl::get().error("RECHAIN_HOME directory doesn't exist!");
            throw std::runtime_error("RECHAIN_HOME directory doesn't exist!");
        }
	}
	else {
        rl::get().error("RECHAIN_HOME environment variable isn't set!");
        throw std::runtime_error("RECHAIN_HOME environment variable isn't set!");
	}

}

Settings* Settings::instance(){
	static Settings settings;
	return &settings;
}

template <typename T>
T Settings::get( std::string key ){
	std::string result = this->settings.at(key);
	return boost::lexical_cast<T>(result);
}

template <typename T>
void Settings::set( std::string key, T value ){
	std::string result = boost::lexical_cast<std::string>(value);
	this->settings[key] = result;
}

bool Settings::save(){
	// save the config
    std::ofstream ofs(this->get<std::string>("config"));
    if(ofs.is_open()){

        // serialize settings to the file
        cereal::JSONOutputArchive archive(ofs);
        archive( *this );

        rl::get().debug("Settings was saved");
        return true;
    }
    rl::get().warning("Settings couldn't be saved");
    return false;
}

bool Settings::load(){
    std::ifstream ifs(this->get<std::string>("config"));
    if(ifs.is_open()){

        // serialize settings from the file
        cereal::JSONInputArchive archive(ifs);
        archive( *this );

        rl::get().debug("Settings was loaded");
        return true;
    }
    rl::get().warning("Settings couldn't be loaded");
    return false;
}
