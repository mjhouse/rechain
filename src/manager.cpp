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
#include <memory>
#include <stdexcept>

// dependency includes
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// local includes
#include "manager.hpp"
#include "record.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logger.hpp"
#include "config.hpp"
#include "remote.hpp"
#include "utility.hpp"

namespace fs = boost::filesystem;


// ----------------------------------------------------------------------------
// Name: 
//      Constructor
// Description:
//      Construct a Manager instance
// ----------------------------------------------------------------------------
Manager::Manager() : m_configured(false), m_blockchain() {
    private_key = std::make_shared<PrivateKey>(PrivateLey::empty());
    private_key->generate();
}

// ----------------------------------------------------------------------------
// Name: 
//      Destructor
// Description:
//      Called when Manager is destroyed
// ----------------------------------------------------------------------------
Manager::~Manager(){}

// ----------------------------------------------------------------------------
// Name: 
//      Destructor
// Description:
//      Called when Manager is destroyed
// ----------------------------------------------------------------------------
bool Manager::setup(){}

// ----------------------------------------------------------------------------
// Name: 
//      configure
// Description:
//      Configure the Manager instance and ensure that the
//      install rechain environment is good.
// ----------------------------------------------------------------------------
bool Manager::configure( Level level ){

    configured = false;

    std::string private_key_path = RCSETTING("private_key");
    std::string blockchain_path  = RCSETTING("blockchain");

    try {

        // verify or create home dir structure
        if(!setup()){
            return false;
        }

        // configure logger
        Logger::get()
            .with( Log("console",STDOUT,level) )
            .with( Log("log",config->setting("log"),Level::error) );

        // load the blockchain or create a new one
        if(!blockchain.load(blockchain_path)){
            blockchain.save(blockchain_path);
        }
        
        // load the private key or create a new one
        if(fs::exists(private_key_path)){
            private_key.reset(PrivateKey::load_file(private_key_path)); 
        }
        else {
            private_key->save(private_key_path);
        }

    }
    catch(const std::exception& e){
        RCERROR("exception during configuration: " + e.what());
        return false;
    }

    configured = true;
    return true;
}

// ----------------------------------------------------------------------------
// Name: 
//      publish
// Description:
//      Publish a document to the network/blockchain.
// ----------------------------------------------------------------------------
bool Manager::publish( std::string t_path ){

    bool result = false;

    try {
        if(configured){

            std::shared_ptr<PublicationRecord> record(new PublicationRecord(t_path));

            if(!record->get_reference().empty()){

                private_key->sign(r);

                result = ( m_blockchain.publish(record &&
                           m_blockchain.is_valid()     &&
                           m_blockchain.save()         && );
                }
                
            }

        }

    }
    catch(const std::exception& e){
        RCERROR("exception during publishing: " + e.what());
        result = false;
    }

    return result;
}

// ----------------------------------------------------------------------------
// Name: 
//      sign
// Description:
//      Sign a previously published document. 
// ----------------------------------------------------------------------------
bool sign( std::string t_hash );

// ----------------------------------------------------------------------------
// Name: 
//      key
// Description:
//      Load the active identity/private key from a path. 
// ----------------------------------------------------------------------------
bool key( std::string t_path );

// ----------------------------------------------------------------------------
// Name: 
//      save
// Description:
//      Save the Blockchain. 
// ----------------------------------------------------------------------------
bool save();

// ----------------------------------------------------------------------------
// Name: 
//      save
// Description:
//      Save the Blockchain to a location. 
// ----------------------------------------------------------------------------
bool save( std::string t_path );

// ----------------------------------------------------------------------------
// Name: 
//      load
// Description:
//      Load the Blockchain. 
// ----------------------------------------------------------------------------
bool load();

// ----------------------------------------------------------------------------
// Name: 
//      load
// Description:
//      Load the Blockchain from a location. 
// ----------------------------------------------------------------------------
bool load( std::string t_path );

// ----------------------------------------------------------------------------
// Name: 
//      is_valid
// Description:
//      Check that the current Blockchain is valid. 
// ----------------------------------------------------------------------------
bool is_valid();

















// publish a file given a path string
bool Manager::publish( std::string t_path ){

    if(configured){

        try {

            PublicationRecord record(new R(t_path));

            if(!record->get_reference().empty()){

                private_key->sign(record);

                return ( m_blockchain.publish());

                if(m_blockchain.publish()){
                    if(blockchain.valid() && blockchain.save()){
                        return true;
                    }
                }
                
            }
        }
        catch(const std::invalid_argument& ex){
            R_INFO("failed to create record from path");
        }

    }

    return result;
}

Record* Manager::request( std::string h ){
	for(auto& b : blockchain){
		for(auto& r : b){
			if(r.get_reference() == h){
                // -----------------------
                // torrent the file contained
                // in the record
                // -----------------------
			    return &r;
            }
		}
	}
    return nullptr;
}

bool Manager::mine(){

    if(configured){

        std::string hash = blockchain.mine(public_key->to_string());
        if(!hash.empty() && blockchain.valid() && blockchain.save()){
            return true;
        }

    }

    return false;
}

void Manager::set_private_key( PrivateKey* k ){
    if(k->valid()){
        std::string path = config->setting("private_key");

        private_key.reset(k);
        private_key->save(path);
    }
}

void Manager::set_public_key( PublicKey* k ){
    if(k->valid()){
        std::string path = config->setting("public_key");

        public_key.reset(k);
        public_key->save(path);
    }
}

bool Manager::make_home(){
    // build/validate the expected dir structure
    fs::path home(config->setting("home"));
    
    fs::path logs(config->setting("logs"));
    fs::path files(config->setting("files"));
    fs::path torrents(config->setting("torrents"));

    try {
        if( !(fs::exists(logs) || fs::create_directory(logs)) ){
            return false;
        }

        if( !(fs::exists(files) || fs::create_directory(files)) ){
            return false;
        }

        if( !(fs::exists(torrents) || fs::create_directory(torrents)) ){
            return false;
        }
    }
    catch (fs::filesystem_error& e){
        rl::get().error(e.what());
        return false;
    }

    return true;
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

	if(s.empty()){
		throw std::invalid_argument("reference cannot be empty");
	}

    if(configured && !s.empty()){
        for(auto& b : blockchain){
            for(auto& r : b){
                if(r.get_reference() == s){
                    Record record( r.get_reference(), b.hash() );
                    return publish(record);
                }
            }
        }
    }

    return false;
}

bool Manager::validate(){
    if(configured){
        return blockchain.valid();
    }

    return false;
}
