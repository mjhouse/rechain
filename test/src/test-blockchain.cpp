#include <iostream>
#include <fstream>
#include <memory>

#include <boost/archive/text_iarchive.hpp>
#include "test-framework.hpp"

#include "blockchain.hpp"
#include "genesis_record.hpp"
#include "publication_record.hpp"
#include "signature_record.hpp"
#include "enums.hpp"
#include "keys.hpp"

test_set blockchain_tests("tests for the blockchain",{

    {"call blockchain default constructor",[]{

        try {
            Blockchain blockchain;
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

    }},
    
    {"call blockchain destructor",[]{

        try {
            Blockchain* blockchain = new Blockchain();
            delete blockchain;
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

    }},
    
    {"publish a genesis record",[]{

        std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/rsa.private")));
        std::vector<std::string> dist = {"FIRST","SECOND"};

        Blockchain blockchain;
        bool result = false;

        // create and publish a valid genesis record
        std::shared_ptr<GenesisRecord> genesis(new GenesisRecord());
        genesis->set_distribution(dist);

        result = blockchain.publish(genesis, private_key);
        RCREQUIRE(result);

        // get it back by hash
        auto first = blockchain.find_record(genesis->hash());
        RCREQUIRE(first);

        // convert to GenesisRecord
        auto fetched = std::dynamic_pointer_cast<GenesisRecord>(first);
        RCREQUIRE(fetched);

        // compare to the original
        RCREQUIRE(blockchain.size() == 1);
        RCREQUIRE(fetched->is_valid());
        RCREQUIRE(fetched->to_string() == genesis->to_string());

        RCREQUIRE(blockchain.is_valid());
    }},

    {"publish a publication record",[]{

        std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/rsa.private")));
        std::vector<std::string> dist = {"FIRST","SECOND"};

        Blockchain blockchain;
        bool result = false;

        // create and publish a valid genesis record
        std::shared_ptr<GenesisRecord> genesis(new GenesisRecord());
        genesis->set_distribution(dist);

        result = blockchain.publish(genesis, private_key);
        RCREQUIRE(result);

        // create and publish a valid publication record
        std::shared_ptr<PublicationRecord> publication(new PublicationRecord(get_path("files/general/test_publication.txt")));

        result = blockchain.publish(publication,private_key);
        RCREQUIRE(result);

        RCREQUIRE(blockchain.is_valid());
    }},

    {"publish a signature record",[]{

        std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/rsa.private")));
        std::vector<std::string> dist = {"FIRST","SECOND"};

        Blockchain blockchain;
        bool result = false;

        // create and publish a valid genesis record
        std::shared_ptr<GenesisRecord> genesis(new GenesisRecord());
        genesis->set_distribution(dist);

        result = blockchain.publish(genesis, private_key);
        RCREQUIRE(result);

        // create and publish a valid publication record
        std::shared_ptr<PublicationRecord> publication(new PublicationRecord(get_path("files/general/test_publication.txt")));

        result = blockchain.publish(publication,private_key);
        RCREQUIRE(result);

        // create and publish a valid signature record
        std::shared_ptr<SignatureRecord> signature(new SignatureRecord(publication->hash()));

        result = blockchain.publish(signature,private_key);
        RCREQUIRE(result);

        RCREQUIRE(blockchain.is_valid());

    }},

    {"find records by hash, and publications by reference",[]{

        Blockchain blockchain;
        blockchain.load(get_path("files/gold/test_blockchain_find.gold"));

        for(auto record : blockchain){
            if(record->get_type() == RecordType::Publication){
                auto pub = std::dynamic_pointer_cast<PublicationRecord>(record);
                auto fetch_pub = blockchain.find_publication(pub->get_reference());
                RCREQUIRE(fetch_pub->to_string() == record->to_string());
            }

            auto fetch_rec = blockchain.find_record(record->hash());
            RCREQUIRE(fetch_rec->to_string() == record->to_string());
        }


    }},

    {"find signatures by reference",[]{

        std::shared_ptr<PrivateKey> private_key(PrivateKey::load_file(get_path("keys/rsa.private")));
        std::vector<std::string> dist = {"FIRST","SECOND"};

        Blockchain blockchain;
        bool result = false;

        // create and publish a valid genesis record
        std::shared_ptr<GenesisRecord> genesis(new GenesisRecord());
        genesis->set_distribution(dist);

        result = blockchain.publish(genesis, private_key);
        RCREQUIRE(result);

        // create and publish a signature record
        std::shared_ptr<PublicationRecord> publication(new PublicationRecord());
        publication->set_reference(generate_hash());

        result = blockchain.publish(publication,private_key);
        RCREQUIRE(result);

        std::vector< std::shared_ptr<SignatureRecord> > signatures;
        for(int i = 0; i < 10; i++){
            // create and publish a valid publication record
            std::shared_ptr<SignatureRecord> signature(new SignatureRecord(publication->hash()));
            result = blockchain.publish(signature,private_key);
            RCREQUIRE(result);

            signatures.push_back(signature);
        }

        auto fetched = blockchain.find_signatures(publication->get_reference());
        for(unsigned int i = 0; i < fetched.size(); ++i){
           auto saved_sig = signatures[i];
           auto fetch_sig = fetched[i];

           RCREQUIRE(saved_sig->to_string() == fetch_sig->to_string());

        }


    }},

    {"check blockchain is valid with no genesis block",[]{

        Blockchain blockchain;
        RCREQUIRE(!blockchain.is_valid());

    }},

    {"check blockchain is valid with published records",[]{

        Blockchain blockchain;
        blockchain.load(get_path("files/gold/test_blockchain_valid.gold"));

        RCREQUIRE(blockchain.is_valid());

    }},

    {"check trust on a valid blockchain",[]{


    }},

});
