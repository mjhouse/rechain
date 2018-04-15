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
#include <boost/bind.hpp>

#include <cereal/archives/json.hpp>


// local includes
#include "remote.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "utility.hpp"

namespace fs = boost::filesystem;
namespace rc = rechain;

using boost::asio::ip::tcp;

typedef Logger rl;

Remote::Remote() : config(nullptr), m_io_service(), m_socket(m_io_service) {
}

Remote::~Remote(){
}

bool Remote::initialize( std::shared_ptr<Config> cfg ){
    config = cfg;

    return true;
}

std::map<std::string,std::string> Remote::get_peers(){
    std::map<std::string,std::string> peers;
    peers.insert( std::make_pair<std::string,std::string>("localhost","8080") );
    return peers;
}

void Remote::listen(){
    m_port = 8080;
    m_endpoint = boost::shared_ptr<tcp::endpoint>(new tcp::endpoint(tcp::v4(), m_port));
    m_acceptor = boost::shared_ptr<tcp::acceptor>(new tcp::acceptor(m_io_service, *m_endpoint));

    m_acceptor->async_accept(m_socket,boost::bind(&Remote::handler,this,boost::asio::placeholders::error)); 
    worker = std::thread([&](boost::asio::io_service* s){ s->run(); }, &m_io_service);
}

void Remote::handler(const boost::system::error_code& error){
    if(!error){
        std::cout << "GOT A MESSAGE" << std::endl;
    }
}

void Remote::stop_listening(){
    m_acceptor->cancel();
    worker.join();
}

unsigned int Remote::send( Record& record ){

    // serialize the record
    std::ostringstream data;
    cereal::JSONOutputArchive archive(data);
    archive( record );
    std::string message = data.str();

    unsigned int result = 0; 

    // for each peer, send a POST request with the 
    // serialized record in the body
    for( auto& peer : get_peers() ){
        std::string address = peer.first;
        std::string port = peer.second;

        // connect to the endpoint
        boost::asio::io_service io_service;

        // get a query and endpoint iterator, and connect
        // a socket to them 
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(address,port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);

        boost::system::error_code ec;
        boost::asio::connect(socket, endpoint_iterator,ec);

        if(ec.value() != ECONNREFUSED){
            // push all the headers into the stream
            boost::asio::streambuf request;
            std::ostream request_stream(&request);

            request_stream << "POST / HTTP/1.1 \r\n";
            request_stream << "Host: localhost:8080\r\n";
            request_stream << "User-Agent: Rechain/1.0\r\n";
            request_stream << "Content-Type: text/plain; charset=utf-8 \r\n";
            request_stream << "Accept: */*\r\n";
            request_stream << "Content-Length: " << message.length() << "\r\n";    
            request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed
            request_stream << message;

            boost::asio::write(socket,request);

            // feed the response into a stream
            boost::asio::streambuf response;
            boost::asio::read_until(socket, response, "\r\n");
            std::istream response_stream(&response);

            // get the response status code
            std::string http_version;
            unsigned int status_code;

            response_stream >> http_version;
            response_stream >> status_code;

            // if any of the peers return 200, the send
            // operation was a success
            if(status_code == 200){
                result++;
            }
        }

    }

    return result;
}
