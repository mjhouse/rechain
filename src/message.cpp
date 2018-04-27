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
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>

// local includes
#include "message.hpp"

int Request::parse_header( std::string s_header ){

    std::vector<std::string> header;
    std::vector<std::string> request;

    // split the received data into lines and split the first line
    // of the header into method, url and version
    boost::algorithm::split_regex( header, s_header, boost::regex("(\r\n|\n)") );
    boost::split(request, header[0], boost::is_any_of("\t "));

    m_method   = request[0];
    m_url      = request[1];
    m_version  = request[2];
    
    // get the remaining header properties and save them
    for(size_t i = 1; i < header.size(); ++i){
        std::vector<std::string> property;
        iter_split(property, header[i], boost::algorithm::first_finder(": "));

        if(property.size() >= 2){
            std::string key = property[0];
            std::string value = property[1];

            boost::algorithm::to_lower(key);

            m_header[key] = value;
        }
    }

    return (header.size() - 1);
}

void Request::read(boost::shared_ptr<boost::asio::ip::tcp::socket> socket){
   
    boost::asio::streambuf buf;
    boost::asio::read_until(*socket, buf, "\r\n\r\n");
    std::istream stream(&buf);

    // get the the first chunk, including the header 
    std::string chunked(std::istreambuf_iterator<char>(&buf), {});

    std::vector<std::string> tmp;
    iter_split(tmp, chunked, boost::algorithm::first_finder("\r\n\r\n"));

    // if we got something assume the first index
    // is the header
    if(tmp.size() > 1){
        std::string s_header = tmp[0];
        m_body.append(tmp[1]); // add the remainder to the body

        // parse the header values
        parse_header(s_header);

        size_t content_length = property<size_t>("Content-Length");
        content_length -= m_body.length();

        // read the remainder of the message into the body
        boost::asio::read(*socket, buf, boost::asio::transfer_exactly(content_length));
        std::string remainder(std::istreambuf_iterator<char>(&buf), {});
         
        m_body.append(remainder);
    }
}

std::string Request::property( std::string key ){
    boost::algorithm::to_lower(key);
    return m_header.at(key);
}

template <typename T>
T Request::property( std::string key ){
    return boost::lexical_cast<T>( property(key) );
}
