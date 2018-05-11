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
#include <utility>
#include <math.h>
#include <climits>

// dependency includes
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// local includes
#include "blockchain.hpp"
#include "base_record.hpp"
#include "genesis_record.hpp"
#include "publication_record.hpp"
#include "signature_record.hpp"
#include "logger.hpp"
#include "enums.hpp"

// ----------------------------------------------------------------------------
// Name: 
//      Constructor
// Description:
//      Construct a Blockchain
// ----------------------------------------------------------------------------
Blockchain::Blockchain() : max_trust(1), min_trust(0) {
}

// ----------------------------------------------------------------------------
// Name: 
//      Destructor
// Description:
//      Called when the Blockchain is deleted
// ----------------------------------------------------------------------------
Blockchain::~Blockchain(){}

// ----------------------------------------------------------------------------
// Name: 
//      update_trust
// Description:
//      Update the trust map
// ----------------------------------------------------------------------------
void Blockchain::update_trust(){

    if(m_blockchain.size() > 0){

        // clear the trust map and set the maximum available
        // trust to the length of the blockchain.
        m_trust.clear();

        // lost trust is the trust that is given
        // to the records. they can't spend it, so it
        // is subtracted from max trust after the trust
        // calculation.
        double lost_trust = 0;

        // get the distribution list from the genesis record and
        // split the max_trust between them.

        GenesisRecord* genesis = static_cast<GenesisRecord*>(&m_blockchain[0]);
        if(!genesis){
            RCERROR("blockchain doesn't have a genesis record");
            return;
        }

        auto distribution = genesis->get_distribution();
        max_trust = static_cast<double>(m_blockchain.size());

        double partial = max_trust/distribution.size();
        for(auto& identifier : distribution){
            m_trust.emplace(identifier,partial);
        }

        // a reference map to link record hashes to authors
        std::map<std::string,std::string> reference;

        for(auto& record : m_blockchain){
       
            switch(record.get_type()){
            
                case RecordType::Publication:
                {

                    PublicationRecord* pub_record = static_cast<PublicationRecord*>(&record);

                    if(pub_record){

                        std::string author = pub_record->get_public_key();
                        std::string hash   = pub_record->hash();

                        // get a reference to the trust of both
                        // the author and the record
                        double& author_trust = m_trust[author];
                        double& record_trust = m_trust[hash];

                        // check the author has trust to give
                        if(author_trust > 0){

                            double half_trust = (author_trust/2);
                           
                            // give the record half of the author's
                            // trust
                            author_trust = half_trust;
                            record_trust = half_trust;

                            // save the lost half that went 
                            // to the record.
                            lost_trust  += half_trust;

                            // link the hash to the author
                            reference.emplace(hash,author);
                        }

                    }

                }
                break;

                case RecordType::Signature:
                {

                    SignatureRecord* sig_record = static_cast<SignatureRecord*>(&record);

                    if(sig_record){

                        // get the signer and the signer's trust
                        std::string signer   = sig_record->get_public_key();
                        double& signer_trust = m_trust[signer];

                        // check that they have trust to give
                        if(signer_trust > 0){

                            std::string hash   = sig_record->get_record_hash();
                            std::string signee = reference[hash];

                            // remove one-half of the signer's trust
                            double quarter_trust = (signer_trust/4);
                            signer_trust = quarter_trust*2;

                            // signee and the record each get one
                            // quarter of the signer's trust
                            m_trust[signee] += quarter_trust;
                            m_trust[hash]   += quarter_trust;

                            // save the lost quarter that went 
                            // to the record.
                            lost_trust   += quarter_trust;

                        }

                    }

                }
                break;

                default:
                {
                    // genesis
                }
                break;

            }
        
        }

        // max_trust is used for normalizing trust values
        // when they are requested. removing lost trust
        // means that the normalization is relative to free
        // trust in the system only.
        max_trust -= lost_trust;

    }
}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::publish
// Description:
//      Mine and publish a valid record to the blockchain,
//      broadcast to other clients, and return true on success
// ----------------------------------------------------------------------------
bool Blockchain::publish( BaseRecord& t_record ){

    std::string hash = t_record.hash();
    RCDEBUG("publishing record: " + hash);

    if(m_blockchain.size() > 0){

        auto& last = m_blockchain.back();
        t_record.set_previous(last.hash());

    }

    t_record.mine();

    if(t_record.is_valid()){
    
        m_blockchain.push_back(t_record);
        // remote->send( t_record );

        RCINFO("record was published: " + hash);
        return true;
    }

    RCERROR("record was not valid: " + hash);
    return false;

}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::publish
// Description:
//      Mine and publish a valid record to the blockchain,
//      broadcast to other clients, and return true on success
// ----------------------------------------------------------------------------
bool Blockchain::publish( BaseRecord* t_record ){
    BaseRecord& record = *t_record;
    return publish(record);

}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::find_record
// Description:
//      Find a publication with the given hash.
// ----------------------------------------------------------------------------
BaseRecord* Blockchain::find_record( std::string t_hash ){
    
    RCDEBUG("searching for record with hash: " + t_hash );
   
    for(auto& record : m_blockchain){
        if(record.hash() == t_hash){

            RCDEBUG("record was found");
            return &record;
        }
    }

    RCDEBUG("record not found");
    return nullptr;

}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::find_publication
// Description:
//      Find a publication with the given reference.
// ----------------------------------------------------------------------------
PublicationRecord* Blockchain::find_publication( std::string t_reference ){

    RCDEBUG("searching for record with reference: " + t_reference);
   
    for(auto& record : m_blockchain){

        PublicationRecord* pub_record = static_cast<PublicationRecord*>(&record);

        if( pub_record && pub_record->get_reference() == t_reference ){

            RCDEBUG("record was found");
            return pub_record;
        }
    }

    RCDEBUG("record not found");
    return nullptr;

}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::find_signature
// Description:
//      Find all signatures for the given reference
// ----------------------------------------------------------------------------
std::vector<SignatureRecord*> Blockchain::find_signatures( std::string t_reference ){

    RCDEBUG("searching for signatures with reference: " + t_reference);
    std::vector<SignatureRecord*> results;

    auto publication = find_publication(t_reference);

    if(publication){

        std::string hash = publication->hash();

        for(auto& record : m_blockchain){

            SignatureRecord* sig_record = static_cast<SignatureRecord*>(&record);

            if( sig_record && sig_record->get_record_hash() == hash ){

                results.push_back(sig_record);
            
            }
        }

    }

    std::string count = std::to_string(results.size());
    RCDEBUG(count + " signatures found");
    return results;
}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::is_valid
// Description:
//      Check that currently loaded blockchain is valid
// ----------------------------------------------------------------------------
bool Blockchain::is_valid(){
    RCDEBUG("checking if blockchain is valid");

	std::set<std::string> references;
	std::set<std::string> hashes;
    std::string previous;

    for(auto& record : m_blockchain){

		if(record.get_previous() != previous){
            RCERROR("record doesn't reference previous");
			return false;
        }

        if(!record.is_valid()){
            RCERROR("record isn't valid");
			return false;
        }

        // update previous for the next iteration
        previous = record.hash();

        switch(record.get_type()){

            case RecordType::Publication:
            {	

                PublicationRecord* pub_record = static_cast<PublicationRecord*>(&record);
                
                if(!pub_record){
                    RCERROR("record type is publication, record is not");
                    return false;
                }

                // try to insert the reference and hash
                auto r_pair = references.insert(pub_record->get_reference());
                auto h_pair = hashes.insert(pub_record->hash());

                // second item in pair is false if insert failed,
                // and both pairs should insert successfully
                if(!r_pair.second && !h_pair.second){
                    RCERROR("duplicate records in blockchain");
                    return false;
                }

            }	
            break;

            case RecordType::Signature:
            {

                SignatureRecord* sig_record = static_cast<SignatureRecord*>(&record);

                if(!sig_record){
                    RCERROR("record type is signature, record is not");
                    return false;
                }

                // try to find the referenced record by reference
                auto it = hashes.find(sig_record->get_record_hash());

                if(it == hashes.end()){
                    RCERROR("signature doesn't reference pre-existing publication");
                    return false;
                }

            }
            break;
        }
    }

    // all tests passed
    return true;
}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::trust
// Description:
//      Get the trust for a publication or user
// ----------------------------------------------------------------------------
double Blockchain::trust( std::string t_identifier ){

    auto it = m_trust.find(t_identifier);

    if( it != m_trust.end() && max_trust > 0 ){
        double trust = it->second;
        return 100*(trust/max_trust);
    }
    else {
        return 0;
    }

}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::begin
// Description:
//      Get iterator to the beginning of the blockchain
// ----------------------------------------------------------------------------
Blockchain::iterator Blockchain::begin(){
	return m_blockchain.begin();
}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::end
// Description:
//      Get iterator to the end of the blockchain
// ----------------------------------------------------------------------------
Blockchain::iterator Blockchain::end(){
	return m_blockchain.end();
}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::size
// Description:
//      Get the size of the current blockchain
// ----------------------------------------------------------------------------
size_t Blockchain::size(){
	return m_blockchain.size();
}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::save
// Description:
//      Serialize the blockchain to disk
// ----------------------------------------------------------------------------
bool Blockchain::save( std::string t_path ){

    RCDEBUG("saving to location: " + t_path);
    std::ofstream os(t_path);

    if(os.is_open()){

        boost::archive::text_oarchive archive(os);
        archive << *this;

        RCINFO("blockchain was saved");
        return true;

    }

    RCWARNING("blockchain failed to save");
	return false;
}

// ----------------------------------------------------------------------------
// Name: 
//      Blockchain::load
// Description:
//      Load a serialized blockchain to memory
// ----------------------------------------------------------------------------
bool Blockchain::load( std::string t_path ){

    RCDEBUG("loading from location: " + t_path);
    std::ifstream is(t_path);

    if(is.is_open()){

        boost::archive::text_iarchive archive(is);
        archive >> *this;

        if(is_valid()){
            update_trust();

            RCINFO("blockchain was loaded");
            return true;
        }
        else {
            RCWARNING("blockchain loaded, but is invalid");
            return false;
        }
    }

    RCWARNING("blockchain failed to load");
	return false;
}
