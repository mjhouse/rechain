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

// dependency includes
#include <boost/filesystem/path.hpp>

// local includes
#include "remote.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "utility.hpp"

namespace fs = boost::filesystem;
//namespace bn = boost::network;
namespace rc = rechain;

typedef Logger rl;
typedef Config cfg;

Remote::Remote(){

}

Remote::~Remote(){

}

void Remote::broadcast( Record& record ){
    // broadcast a new record to the network
    //std::string url = Config::get()->setting("tracker");
}

void Remote::receive( ){
}
