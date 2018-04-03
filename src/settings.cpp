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
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

// dependency includes
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

Settings::Settings(){

	// find the users home directory
	const char *home;
	fs::path root;
	if(( home = getenv("RECHAIN_HOME")) != NULL){
		// if RECHAIN_HOME is set, use that
		root = fs::path(std::string(home));
	}
	else if (( home = getenv("XDG_CONFIG_HOME"))  != NULL ||
	         ( home = getenv("HOME"))             != NULL ||
	 	     ( home = getpwuid(getuid())->pw_dir) != NULL )
	{
		// otherwise try XDG_CONFIG_HOME, HOME and getpwuid(),
		// and look for '.rechain' in the home directory
		root = fs::path(std::string(home));
		root /= ".rechain";
	}
	else {
		// if nothing else works- throw an error
	}

	if(fs::exists(root) && fs::is_directory(root)){
		// make sure subdirs exist
		this->set<std::string>("HOME",root.string());
		this->set<std::stri>
		if(fs::exists(root / "rechain.config")){
			
		}
	}
	else {
		// throw an error: home directory doesn't exist
	}

}

Settings* Settings::instance(){
	static Settings settings;
	return &settings;
}

<template typename T>
T Settings::get( std::string key ){
	std::string result = this->settings.at(key);
	return boost::lexical_cast<T>(result);
}

<template typename T>
void Settings::set( std::string key, T value ){
	std::string result = boost::lexical_cast<std::string>(value);
	this->settings[key] = result;
}

bool Settings::save(){
	// save the config
}

bool Settings::load(){
	// load the config
}
