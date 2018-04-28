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
#include "config.hpp"
#include "utility.hpp"
#include "logger.hpp"

namespace fs = boost::filesystem;
typedef Logger rl;

Config::Config() : initialized(false) {

}

Config::~Config(){

}

void Config::set_peers( std::map<std::string,std::string> peers ){
    m_peers = peers;
}

std::map<std::string,std::string> Config::get_peers(){
    return m_peers;
}

bool Config::initialize(){

	// find the users home directory
    if(!initialized){
        const char* path = std::getenv("RECHAIN_HOME");
        if( path != NULL && (std::strlen(path) != 0)){

            fs::path home(path);

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
            setting("home",home.string());
            setting("config",config.string());
            setting("public_key",public_key.string());
            setting("private_key",private_key.string());
            setting("log",log.string());
            setting("blockchain",blockchain.string());

            setting("logs",logs.string());
            setting("files",files.string());
            setting("torrents",torrents.string());

            initialized = true;
            return initialized;
        }
	}

    return false;

}

std::string Config::setting( std::string key ){
    auto it = settings.find(key);
    if(it == settings.end())
        return "";

    return it->second;
}

void Config::setting( std::string key, std::string value ){
    this->settings[key] = value;
}

bool Config::save( std::string path ){
    setting("config",path);
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

bool Config::save(){
    return save(setting("config"));
}

bool Config::load( std::string path ){
    setting("config",path);
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

bool Config::load(){
    return load(setting("config"));
}

