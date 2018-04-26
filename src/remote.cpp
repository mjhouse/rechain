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
#include <regex>

// dependency includes
#include <boost/filesystem/path.hpp>
#include <boost/make_shared.hpp>
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

using namespace boost::asio::ip;

typedef Logger rl;

Remote::Remote(std::shared_ptr<Config> cfg) 
    : config(cfg), service(), service_thread() {

}

Remote::~Remote(){
}

void Remote::handle( boost::shared_ptr<tcp::socket> socket ){

    // feed the request into a stream
    boost::asio::streambuf request;
    boost::asio::read_until(*socket, request, "\r\n\r\n");
    std::istream response_stream(&request);

    // get the message header 
    std::string header(std::istreambuf_iterator<char>(response_stream), {});
    std::cout << header << std::endl;

    /*
    std::regex rgx("Content-Length: (\d).*");
    std::smatch match;

    if (std::regex_search(header.begin(), header.end(), match, rgx))
        std::cout << "match: " << match[1] << '\n';
    else:
        std::cout << "no match" << std::endl;
    */

    // -----------------------------------
    // send a '200' response to the client
    boost::asio::streambuf response;
    std::ostream stream(&response);

    stream << "HTTP/1.1 200 OK\r\n";
    stream << "Connection: close\r\n\r\n"; 

    boost::asio::write(*socket,response);

    listen();
}

void Remote::listen(){
    boost::shared_ptr<tcp::socket> socket(new tcp::socket(service));
    acceptor->async_accept(*socket,boost::bind(&Remote::handle,this,socket));
}

void Remote::start_listening(){
    if(service_thread) return;

    acceptor = boost::make_shared<tcp::acceptor>(service,tcp::endpoint(tcp::v4(),8080));

    listen();

    service_thread.reset(new boost::thread(
        boost::bind(&boost::asio::io_service::run,&service)
    ));
}

void Remote::stop_listening(){
    if(!service_thread) return;

    acceptor->cancel();

    service.stop();
    service_thread->join();

    service.reset();
    service_thread.reset();
}

unsigned int Remote::send( Record& record ){

    // serialize the record
    std::ostringstream data;
    cereal::JSONOutputArchive archive(data);
    archive( record );
    std::string message = data.str();

    // ----------------------------------------------------
    // TEST CODE:
    //      peers will be in config, and this is a stand in
    //      for a collection of peers received
    std::map<std::string,std::string> peers;
    peers.insert( std::make_pair<std::string,std::string>("localhost","8080") );
    // ----------------------------------------------------

    unsigned int result = 0; 

    // for each peer, send a POST request with the 
    // serialized record in the body
    for( auto& peer : peers ){
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

            size_t bufsize = request.size();

            size_t written = boost::asio::write(socket,request);
            std::cout << written << " : " << bufsize << std::endl;

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
