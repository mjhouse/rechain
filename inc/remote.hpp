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
#include "config.hpp"
#include "keys.hpp"

namespace fs = boost::filesystem;

/** \brief Handles http(s) requests and torrents.
*/
class Remote {

    private:

        /** The settings for the application */
        std::shared_ptr<Config> config;

    public:
        /** Private constructor 
        */
        Remote();

        /** Destructor
        */
        ~Remote();

        /** Init the Remote object 
            \param cfg Application configuration
        */
        bool initialize( std::shared_ptr<Config> cfg );
        
        /** Broadcast a new record to all miners 
            \param record The Record to broadcast
        */
        void broadcast( Record& record );

        /** Push received records to a listener 
        */
        void receive();

};

#endif
