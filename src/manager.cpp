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
#include <sstream>
#include <string>

// dependency includes
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

Manager::Manager(){
}

Manager::~Manager(){
    if(blockchain.valid()){
        blockchain.save();
    }
}

bool Manager::configure( Level level ){
    
    settings.reset(Settings::instance());
    if(!settings->initialize())
        return false;

    Logger::get()
        .with( Log("console",STDOUT,level) )
        .with( Log("log",settings->gets("log"),Level::error) );

    std::string private_key_path = settings->gets("private_key");
    std::string public_key_path  = settings->gets("public_key");
    std::string blockchain_path  = settings->gets("blockchain");

    if(!this->blockchain.load(blockchain_path)){
        this->blockchain.save(blockchain_path);
    }
    
    // if there is no private key or key file, create a new one
    if(!this->private_key && !fs::exists(private_key_path)){
       this->private_key.reset(PrivateKey::empty());
       this->private_key->generate();

       this->private_key->save(private_key_path);
    }

    // if there is no public key or key file, create a new one
    if(!this->public_key && !fs::exists(public_key_path)){
       this->public_key.reset(PublicKey::empty());
       this->public_key->generate(this->private_key.get());

       this->public_key->save(public_key_path);
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
        // -----------------------
        // seed the file contained
        // in the record
        // -----------------------
        Record r(ifs);
        return this->publish(r);
    }

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

bool Manager::mine(){
    blockchain.mine();
    if(blockchain.valid() && blockchain.save())
        return true;
    return false;
}

void Manager::set_private_key( PrivateKey* k ){
    if(k->valid()){
        private_key.reset(k);
        private_key->save(settings->gets("private_key"));
    }
}

void Manager::set_public_key( PublicKey* k ){
    if(k->valid()){
        public_key.reset(k);
        public_key->save(settings->gets("private_key"));
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
