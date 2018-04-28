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
#include <stdexcept>

// dependency includes
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>

// local includes
#include "message.hpp"

Message::Message() : m_header(), m_body(), m_version("HTTP/1.1") {
}

Message::~Message(){
}

int Message::parse_header( std::string s_header ){

    std::vector<std::string> header;
    std::vector<std::string> request;

    // split the received data into lines and split the first line
    // of the header into method, url and version
    boost::algorithm::split_regex( header, s_header, boost::regex("(\r\n|\n)") );
    boost::split(request, header[0], boost::is_any_of("\t "));

    // parse Response/Request specific values
    set_specific(request);

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

void Message::read(boost::asio::ip::tcp::socket& socket){
   
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\r\n\r\n");
    std::istream stream(&buf);

    // get the the first chunk, including the header 
    std::string chunked(std::istreambuf_iterator<char>(&buf), {});

    std::vector<std::string> tmp;
    iter_split(tmp, chunked, boost::algorithm::first_finder("\r\n\r\n"));

    // if we got something assume the first index
    // is the header
    if(tmp.size() > 0){
        std::string s_header = tmp[0];

        // parse the header values
        parse_header(s_header);

        size_t content_length = get_property<size_t>("Content-Length");

        if(content_length > 0){
            
            if(tmp.size() > 1)
                m_body.append(tmp[1]); // add the remainder to the body

            content_length -= m_body.length();

            // read the remainder of the message into the body
            boost::asio::read(socket, buf, boost::asio::transfer_exactly(content_length));
            std::string remainder(std::istreambuf_iterator<char>(&buf), {});
             
            m_body.append(remainder);

        }
    }
}

std::string Message::serialize(){
    std::string result = get_specific();

    for(auto& pair : m_header){
        std::string key   = pair.first;
        std::string value = pair.second;

        result.append(key + ": " + value + "\r\n");
    }

    result.append("\r\n");
    
    if(!m_body.empty()){
        result.append(m_body + "\r\n\r\n");
    }

    return result;
}

void Message::write(boost::asio::ip::tcp::socket& socket){
    boost::asio::streambuf request;
    std::ostream stream(&request);

    stream << serialize();

    boost::asio::write(socket,request);
}

std::string Message::get_property( std::string key ){
    boost::algorithm::to_lower(key);
    
    if(m_header.find(key) != m_header.end())
        return m_header.at(key);

    return "";
}

void Message::set_property( std::string key, std::string value ){
    boost::algorithm::to_lower(key);
    m_header[key] = value;
}

template <typename T>
T Message::get_property( std::string key ){
    std::string value = get_property(key);
    
    if(!value.empty())
        return boost::lexical_cast<T>( value );
    
    return T();
}

// ----------------------------------------------------------------------------
// REQUEST

Request::Request() : m_method("GET"), m_url() {
}

Request::~Request(){
}

void Request::set_specific( std::vector<std::string> specific ){
    if(specific.size() < 3)
        throw std::invalid_argument("not enough values");

    m_method   = specific[0];
    m_url      = specific[1];
    m_version  = specific[2];
}

std::string Request::get_specific(){
    return m_method + " " + m_url + " " + m_version + "\r\n";
}

void Request::clear(){
    m_header.clear();
    m_body.clear();
    m_method = "GET";
    m_url.clear();
    m_version = "HTTP/1.1";
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// RESPONSE

Response::Response() : m_code(200), m_status("OK") {
}

Response::~Response(){
}

void Response::set_specific( std::vector<std::string> specific ){
    if(specific.size() < 3)
        throw std::invalid_argument("not enough values");

    m_version  = specific[0];
    m_code     = boost::lexical_cast<int>(specific[1]);
    m_status   = specific[2];
}

std::string Response::get_specific(){
    return m_version + " " + std::to_string(m_code) + " " + m_status + "\r\n";
}

void Response::clear(){
    m_header.clear();
    m_body.clear();
    m_version = "HTTP/1.1";
    m_code    = 200;
    m_status  = "OK";
}

// ----------------------------------------------------------------------------
