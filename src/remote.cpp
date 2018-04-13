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
#include <boost/asio.hpp>


// local includes
#include "remote.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "utility.hpp"

namespace fs = boost::filesystem;
namespace rc = rechain;

using boost::asio::ip::tcp;

typedef Logger rl;

Remote::Remote() : config(nullptr) {
}

Remote::~Remote(){

}

bool Remote::initialize( std::shared_ptr<Config> cfg ){
    config = cfg;
    return true;
}

bool Remote::send( Record& record ){
    // broadcast a new record to the network
    std::string ip_address= config->setting("tracker");
    std::string message = "TEST";


    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query( ip_address, "80" );
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);


    boost::asio::streambuf request;
    std::ostream request_stream(&request);

    request_stream << "POST /title/ HTTP/1.1 \r\n";
    request_stream << "Host: http://google\r\n";
    request_stream << "User-Agent: C/1.0";
    request_stream << "Content-Type: text/plain; charset=utf-8 \r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Length: " << message.length() << "\r\n";    
    request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed
    request_stream << message;

    boost::asio::write(socket,request);

    // get the response
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    std::istream response_stream(&response);

    // set up the input variables
    std::string http_version;
    unsigned int status_code;
    std::string status_message;

    response_stream >> http_version;
    response_stream >> status_code;

    std::getline(response_stream,status_message);

    std::cout << http_version << "\n" 
              << status_code << "\n"
              << status_message << "\n"
              << std::endl;
    return true;
}

void Remote::receive( ){
}
