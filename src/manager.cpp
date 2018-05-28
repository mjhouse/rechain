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
    m_private_key = std::make_shared<PrivateKey>(PrivateKey::empty());
    m_private_key->generate();
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
//      Configure or validate the home directory
// ----------------------------------------------------------------------------
bool Manager::setup(){

    fs::path home(Config::get()->setting("home"));
    
    fs::path logs(Config::get()->setting("logs"));
    fs::path files(Config::get()->setting("files"));
    fs::path torrents(Config::get()->setting("torrents"));

    try {
        if( !(fs::exists(logs) || fs::create_directory(logs)) ){
            RCERROR("failed to create 'log' directory during setup");
            return false;
        }

        if( !(fs::exists(files) || fs::create_directory(files)) ){
            RCERROR("failed to create 'files' directory during setup");
            return false;
        }

        if( !(fs::exists(torrents) || fs::create_directory(torrents)) ){
            RCERROR("failed to create 'torrents' directory during setup");
            return false;
        }
    }
    catch (fs::filesystem_error& e){
        RCERROR(e.what());
        return false;
    }

    return true;
    
}

// ----------------------------------------------------------------------------
// Name: 
//      configure
// Description:
//      Configure the Manager instance and ensure that the
//      install rechain environment is good.
// ----------------------------------------------------------------------------
bool Manager::configure( Level level ){

    m_configured = false;

    std::string private_key_path = Config::get()->setting("private_key");
    std::string blockchain_path  = Config::get()->setting("blockchain");

    try {

        // verify or create home dir structure
        if(!setup()){
            return false;
        }

        // verify or create home dir structure
        if(!Config::get()->initialize()){
            return false;
        }

        // configure logger
        Logger::get()
            .with( Log("console",STDOUT,level) )
            .with( Log("log",Config::get()->setting("log"),Level::error) );

        // load the blockchain or create a new one
        if(!m_blockchain.load(blockchain_path)){
            m_blockchain.save(blockchain_path);
        }
        
        // load the private key or create a new one
        if(fs::exists(private_key_path)){
            m_private_key.reset(PrivateKey::load_file(private_key_path)); 
        }
        else {
            m_private_key->save(private_key_path);
        }

    }
    catch(const std::exception& e){
        RCERROR(e.what());
        return false;
    }

    m_configured = true;
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
        if(m_configured && fs::exists(t_path)){

            std::shared_ptr<PublicationRecord> record(new PublicationRecord(t_path));

            std::string path = Config::get()->setting("blockchain");

            result = ( m_blockchain.publish(record,m_private_key) &&
                       m_blockchain.is_valid()                    &&
                       m_blockchain.save(path)                      );
        }

    }
    catch(const std::exception& e){
        RCERROR(e.what());
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
bool Manager::sign( std::string t_hash ){
    
	if(t_hash.empty()){
        RCERROR("hash to sign is empty");
        return false;
	}

    if(!m_configured){
        RCERROR("manager is not configured");
        return false;
    }

    auto record = m_blockchain.find_record(t_hash);

    if(record && record->get_type() == RecordType::Publication){

        std::shared_ptr<SignatureRecord> record(new SignatureRecord(t_hash));
        return m_blockchain.publish(record,m_private_key);

    }

    return false;
}

// ----------------------------------------------------------------------------
// Name: 
//      key
// Description:
//      Load the active identity/private key from a path. 
// ----------------------------------------------------------------------------
bool Manager::key( std::string t_path ){

    std::ifstream ifs(t_path);

    if(ifs.is_open()){

        try {

            PrivateKey* key = PrivateKey::load_file(t_path);

            if(key->valid()){

                m_private_key.reset(key);
                m_private_key->save(Config::get()->setting("private_key"));

            }

        } catch (const CryptoPP::InvalidArgument& e){

            RCERROR(e.what());
            return false;

        } catch (const std::invalid_argument& e){

            RCERROR("not a valid private key: " + t_path);
            return false;

        }

    }
    return true;
}

// ----------------------------------------------------------------------------
// Name: 
//      save
// Description:
//      Save the Blockchain. 
// ----------------------------------------------------------------------------
bool Manager::save(){
    return m_blockchain.save(Config::get()->setting("blockchain"));
}

// ----------------------------------------------------------------------------
// Name: 
//      save
// Description:
//      Save the Blockchain to a location. 
// ----------------------------------------------------------------------------
bool Manager::save( std::string t_path ){
    return m_blockchain.save(t_path);
}

// ----------------------------------------------------------------------------
// Name: 
//      load
// Description:
//      Load the Blockchain. 
// ----------------------------------------------------------------------------
bool Manager::load(){
    return m_blockchain.load(Config::get()->setting("blockchain"));
}

// ----------------------------------------------------------------------------
// Name: 
//      load
// Description:
//      Load the Blockchain from a location. 
// ----------------------------------------------------------------------------
bool Manager::load( std::string t_path ){
    return m_blockchain.load(t_path);
}

// ----------------------------------------------------------------------------
// Name: 
//      is_valid
// Description:
//      Check that the current Blockchain is valid. 
// ----------------------------------------------------------------------------
bool Manager::is_valid(){
    if(m_configured){
        return m_blockchain.is_valid();
    }

    return false;
}
