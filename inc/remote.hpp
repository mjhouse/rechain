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
#include <thread>

// dependency includes
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

// local includes
#include "blockchain.hpp"
#include "config.hpp"
#include "keys.hpp"

namespace fs = boost::filesystem;

using namespace boost::asio::ip;

/** \brief Handles http(s) requests and torrents.
*/
class Remote {

    private:

        /** The settings for the application */
        std::shared_ptr<Config> config;

        /** IO service to manage system access */
        boost::asio::io_service service;

        /** The acceptor that listens for broadcasts */
        boost::shared_ptr<tcp::acceptor> acceptor;

        /** A thread that service runs in */
        boost::shared_ptr<boost::thread> service_thread;
        
        /** This flag will be used to control thread listening */
        bool running;


    public:
        /** Private constructor
            \param cfg A pointer to Config
        */
        Remote(std::shared_ptr<Config> cfg);

        /** Destructor
        */
        ~Remote();

        /** \brief Start listening for broadcasts
        */
        void start_listening();

        /** \brief Stop listening for broadcasts
        */
        void stop_listening();

        /** Broadcast a new record to all miners 
            \param record The Record to broadcast
            \returns The number of peers who accepted the record 
        */
        unsigned int send( Record& record );
};

#endif
