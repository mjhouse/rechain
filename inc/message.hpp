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

/** \file   message.hpp
	\brief 	Parses http requests
*/

#ifndef _RECHAIN_MESSAGE_HPP_
#define _RECHAIN_MESSAGE_HPP_

// system includes
#include <iostream>
#include <string>

// dependency includes
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

/** \brief Parses an http request
*/
class Request {
    private:
        /** Header keys and values */
        std::map<std::string,std::string> m_header;

        /** The body of the request */
        std::string m_body;

        /** The method of the request (e.g. 'POST') */
        std::string m_method;

        /** The url of the request */
        std::string m_url;

        /** The http version */
        std::string m_version;

        /** \brief Parse the header of the request
            \param s_header The header to parse
            \returns The number of properties found
        */
        int parse_header( std::string s_header );

    public:
        Request(){}
        
        ~Request(){}

        /** \brief Reads http request data from a socket
            \param socket the open socket to read from
        */
        void read(boost::shared_ptr<boost::asio::ip::tcp::socket> socket);

        /** \brief Get a header property
            \param key The property to get (e.g. 'Content-Type')
            \returns Property value as a string ('text/plain')
        */
        std::string property( std::string key );

        /** \brief Get a header property as type 'T'
            \param key The property to get (e.g. 'Content-Length')
            \returns Property value as a type 'T'
        */
        template <typename T>
        T property( std::string key );

        /** \brief Get the message body
            \returns The body of the request
        */
        std::string body( ){ return m_body; }

        /** \brief Get the request method (e.g. 'POST')
            \returns The request method
        */
        std::string method(){ return m_method; }

        /** \brief Get the request url (e.g. '/')
            \returns The request endpoint
        */
        std::string url(){ return m_url; }

        /** \brief Get the http version string
            \returns The http verison as a string
        */
        std::string version(){ return m_version; }
};

#endif
