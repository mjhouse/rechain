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
class Message {
    protected:
        /** Header keys and values */
        std::map<std::string,std::string> m_header;

        /** The body of the request */
        std::string m_body;

        /** The http version */
        std::string m_version;

        /** \brief Set the Response/Request specific values 
            \param specific A vector of Response/Request specific values
        */
        virtual void set_specific( std::vector<std::string> specific )=0;

        /** \brief Get the Response/Request header line
            \returns The Response/Request specific header line
        */
        virtual std::string get_specific()=0;

        /** \brief Parse the header
            \param s_header The header to parse
            \returns The number of properties found
        */
        int parse_header( std::string s_header );

    public:

        /** \brief The constructor */
        Message();
        
        /** \brief The destructor */
        ~Message();

        /** \brief Convert the Message to a string
            \returns An http Message as a string
        */
        std::string serialize();

        /** \brief Reads http Message data from a socket
            \param socket the open socket to read from
        */
        void read(boost::asio::ip::tcp::socket& socket);

        /** \brief Writes http request data to a socket
            \param socket the open socket to write to
        */
        void write(boost::asio::ip::tcp::socket& socket);

        /** \brief Reset the message to defaults
        */
        virtual void clear()=0;

        /** \brief Get a header property
            \param key The property to get (e.g. 'Content-Type')
            \returns Property value as a string ('text/plain')
        */
        std::string get_property( std::string key );

        /** \brief Set a header property
            \param key The property to set (e.g. 'Content-Type')
            \param value The value to set it to ('text/plain')
        */
        void set_property( std::string key, std::string value );

        /** \brief Get a header property as type 'T'
            \param key The property to get (e.g. 'Content-Length')
            \returns Property value as a type 'T'
        */
        template <typename T>
        T get_property( std::string key );

        /** \brief Get the message body
            \returns The body of the request
        */
        std::string get_body(){ return m_body; }

        /** \brief Set the message body
            \param body The value to use for body
        */
        void set_body( std::string body ){ 
            m_body = body; 
            m_header["content-length"] = std::to_string(body.size());
        }

        /** \brief Get the http version string
            \returns The http verison as a string
        */
        std::string get_version(){ return m_version; }

        /** \brief Set the http version string
            \param version The version string to use
        */
        void set_version( std::string version ){ m_version = version; }
};

/** \brief Parse an http request
*/
class Request : public Message {
    private:

        /** The method of the request (e.g. 'POST') */
        std::string m_method;

        /** The url of the request */
        std::string m_url;

        /** \brief Set the Request specific values 
            \param specific A vector of Request specific values
        */
        void set_specific( std::vector<std::string> specific );

        /** \brief Get the Request header line
            \returns The Request specific header line
        */
        std::string get_specific();

    public:

       Request();

       ~Request();

        /** \brief Reset the message to defaults
        */
        void clear();

        /** \brief Get the request method (e.g. 'POST')
            \returns The request method
        */
        std::string get_method(){ return m_method; }

        /** \brief Set the message method
            \param method The value to use for method (e.g. 'POST')
        */
        void set_method( std::string method ){ m_method = method; }

        /** \brief Get the request url (e.g. '/')
            \returns The request endpoint
        */
        std::string get_url(){ return m_url; }

        /** \brief Set the request url (e.g. '/')
            \param url The value to use for the url
        */
        void set_url( std::string url ){ m_url = url; }

};

/** \brief Parse an http response 
*/
class Response : public Message {
    private:

        /** The status code of the response (e.g. 200) */
        int m_code;

        /** The status message of the response (e.g. 'OK') */
        std::string m_status;

        /** \brief Set the Response specific values 
            \param specific A vector of Response specific values
        */
        void set_specific( std::vector<std::string> specific );

        /** \brief Get the Response header line
            \returns The Response specific header line
        */
        std::string get_specific();

    public:
        
        Response();

        ~Response();

        /** \brief Reset the message to defaults
        */
        void clear();

        /** \brief Get the response code 
            \returns The code as an int
        */
        int get_code(){ return m_code; }

        /** \brief Set the status code
            \param code The value to set the code to
        */
        void set_code( int code ){ m_code = code; }

        /** \brief Get the status message of the reponse
            \returns The status message
        */
        std::string get_status(){ return m_status; }

        /** \brief Set the status message
            \param status The status message to set
        */
        void set_status( std::string status ){ m_status = status; }
};

#endif
