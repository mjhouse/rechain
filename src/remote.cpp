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
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <cereal/archives/json.hpp>


// local includes
#include "remote.hpp"
#include "message.hpp"
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

    // build a Request from the socket
    Request request;
    request.read(*socket);

    std::string body = request.get_body();

    std::stringstream serialized(body);
    Record record;
              
    {
        cereal::JSONInputArchive archive(serialized);
        archive( record );
    }

    if(m_callback)
        m_callback(record);

    // send a '200' response to the client
    Response response;

    response.set_code(200);
    response.set_status("OK");
    response.set_property("Connection","close");

    response.write(*socket);
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

void Remote::send( Record& record ){

    // serialize the record
    std::ostringstream data;
    
    {
        cereal::JSONOutputArchive archive(data);
        archive( record );
    }

    std::string message = data.str();

    // for each peer, send a POST request with the 
    // serialized record in the body
    for( auto& peer : config->get_peers() ){
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
            
            Request res;

            res.set_method("POST");
            res.set_url("/");
            res.set_property("Host","localhost:8080");
            res.set_property("User-Agent","Rechain/1.0");
            res.set_property("Content-Type","text/plain; charset=utf-8");
            res.set_property("Accept","*/*");
            res.set_property("Content-Length",std::to_string(message.length()));
            res.set_property("Message-Type","Publish");
            res.set_property("Connection","close");
            res.set_body(message);

            res.write(socket);
        }

    }
}
