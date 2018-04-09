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

/** \file   remote.hpp
	\brief 	Handles web requests
*/

#ifndef _RECHAIN_REMOTE_HPP_
#define _RECHAIN_REMOTE_HPP_

// system includes
#include <string>
#include <iostream>

// dependency includes
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// local includes
#include "blockchain.hpp"
#include "keys.hpp"

namespace fs = boost::filesystem;

class Settings;

/** \brief Handles http(s) requests and torrents.
*/
class Remote {

    private:
        std::shared_ptr<Settings> settings;


        /** Create a new torrent file
            \param file The file to create a torrent for
            \returns A path to the torrent file
        */
        std::string create_torrent( fs::path file );

    public:

        /** Private constructor 
        */
        Remote( std::shared_ptr<Settings>& s );

        /** Destructor
        */
        ~Remote();

        /** Seed a file to peers 
            \param path The file to seed
        */
        void seed( std::string path );

        /** Get a file from peers
            \param t The file hash 
        */
        void leech( std::string t );
};

#endif
