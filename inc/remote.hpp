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

// local includes
#include "blockchain.hpp"
#include "config.hpp"
#include "keys.hpp"

namespace fs = boost::filesystem;

using boost::asio::ip::tcp;

/** \brief Handles http(s) requests and torrents.
*/
class Remote {

    private:

        /** The settings for the application */
        std::shared_ptr<Config> config;

        /** IO service to manage system access */
        boost::asio::io_service m_io_service;

        /** Thread for m_io_services,run  */
        std::thread worker;

        /** The socket for listening */
        tcp::socket m_socket;

        /** An endpoint to listen at */
        boost::shared_ptr<tcp::endpoint> m_endpoint;

        /** Calls handlers with incoming requests */
        boost::shared_ptr<tcp::acceptor> m_acceptor;

        /** The port number to listen at */
        unsigned short m_port;

        /** \brief Get a list of peers to broadcast to
            \returns A map of address/port pairs
        */
        std::map<std::string,std::string> get_peers();

        /** Push received records to a listener 
            \returns True on success
        */
        void receive();

    public:
        /** Private constructor 
        */
        Remote();

        /** Destructor
        */
        ~Remote();

        /** \brief Init the Remote
            \param cfg A Config pointer to provide paths
            \returns True if initialization was successful
        */
        bool initialize( std::shared_ptr<Config> cfg );

        /** \brief Listen for incoming requests */
        void listen();

        /** \brief Handle requests
            \param error An error code or '0'
        */
        void handler(const boost::system::error_code& error);

        /** \brief Shut down worker thread, cancel acceptor
        */
        void stop_listening();

        /** Broadcast a new record to all miners 
            \param record The Record to broadcast
            \returns The number of peers who accepted the record 
        */
        unsigned int send( Record& record );
};

#endif
