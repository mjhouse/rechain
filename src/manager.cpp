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
 * Contact: michaelhouse@gmx.us
 *
*/

// system includes
#include <iostream>
#include <sstream>
#include <string>

// dependency includes
#include "cxxopts.hpp"
#include <cryptopp/osrng.h>	    // for the AutoSeededRandomPool
#include <cryptopp/integer.h>	// for Integer data type
#include <cryptopp/hex.h>	    // for the HexEncoder

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/assign/list_inserter.hpp>

// local includes
#include "manager.hpp"
#include "record.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logger.hpp"

namespace fs = boost::filesystem;
typedef Logger rl;

#define DBG( s )( std::cout << "DEBUG: " << s << std::endl )

Manager::Manager( std::string home, Level level ){
    /*
    insert( this->context )
        ("home",home)
        ("log",home + "/logs/rechain.log")
        ("private_key");
    */
    this->home = home;
    Logger::get()
        .with( Log("console",STDOUT,level) )
        .with( Log("log",home + "/rechain.log",Level::error) );
}

Manager::~Manager(){
    if(blockchain.valid()){
        blockchain.save();
    }
}

bool Manager::configure(){
    std::string private_key_path = home + "/current.private";
    std::string public_key_path  = home + "/current.public";
    std::string blockchain_path  = home + "/rechain.blockchain";

    if( this->home.empty() ){
        rl::get().error("RECHAIN_HOME isn't set.");
        return false;
    }
    else {
        //fs::create_directories(home + "/torrents/");
        //fs::create_directories(home + "/files/");
        //fs::create_directories(home + "/logs/");

        if(!this->blockchain.load(blockchain_path)){
            this->blockchain.save(blockchain_path);
        }

        if(!this->private_key && !fs::exists(private_key_path)){
           this->private_key.reset(PrivateKey::empty());
           this->private_key->generate();
           
           this->private_key->save(private_key_path);
        }

        if(!this->public_key && !fs::exists(public_key_path)){
           this->public_key.reset(PublicKey::empty());
           this->public_key->generate(this->private_key.get());
           
           this->public_key->save(public_key_path);
        }

    }

    return true;
}

// publish a record object
bool Manager::publish( Record& r ){
    private_key->sign(r);

    if(!blockchain.contains(r.reference(),Search::RecordType)){
        blockchain.add(r);
  
        if(blockchain.valid() && blockchain.save()){
            return true;
        }
    } 

    return false;
}

// publish a file
bool Manager::publish( std::string s ){
    std::ifstream ifs(s);
    if(ifs.is_open()){
        Record r(ifs);
        return this->publish(r);
    }

    return false;
}

bool Manager::mine(){
    blockchain.mine();
    if(blockchain.valid() && blockchain.save())
        return true;
    return false;
}

Record Manager::request( std::string h ){
	for(auto b : blockchain){
		for(auto r : b){
			if(r.reference() == h){
                // -----------------------
                // torrent the file contained
                // in the record
                // -----------------------
			    return r; 
            }
		}
	}
    return Record();
}

void Manager::set_private_key( PrivateKey* k ){
    if(k->valid()){
        private_key.reset(k);
        private_key->save(home + "/current.private");
    }
}

void Manager::set_public_key( PublicKey* k ){
    if(k->valid()){
        public_key.reset(k);
        public_key->save(home + "/current.public");
    }
}

bool Manager::set_private_key( std::string p ){
    std::ifstream ifs(p);
    if(ifs.is_open()){
        try {
            this->set_private_key(PrivateKey::load_file(p));
        } catch (const CryptoPP::InvalidArgument& e){
            rl::get().error(e.what());
            return false;
        } catch (const std::invalid_argument& e){
            rl::get().error("Not a valid private key: " + p);
            return false;
        }
    }
    return true;
}

bool Manager::set_public_key( std::string p ){
    std::ifstream ifs(p);
    if(ifs.is_open()){
        try {
            this->set_public_key(PublicKey::load_file(p));
        } catch (const CryptoPP::InvalidArgument& e){
            rl::get().error(e.what());
            return false;
        } catch (const std::invalid_argument& e){
            rl::get().error("Not a valid public key: " + p);
            return false;
        }
    }
    return true;
}

bool Manager::sign( std::string s ){
	for(auto b : blockchain){
		for(auto r : b){
			if(r.reference() == s){
				Record r( r.reference(), b.hash() );
                return this->publish(r);
			}
		}
	}
    return false;
}

bool Manager::validate(){
    return blockchain.valid();
}
