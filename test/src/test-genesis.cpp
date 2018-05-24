#include <iostream>
#include <fstream>
#include <memory>
#include <csignal>

#include <boost/archive/text_iarchive.hpp>

#include "test-framework.hpp"
#include "genesis_record.hpp"
#include "enums.hpp"
#include "keys.hpp"

test_set genesis_tests("tests for genesis records",{

    {"call genesis record default constructor",[]{

        try {
            GenesisRecord gr;
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

    }},

    {"test the genesis record destructor",[]{

        try {
            GenesisRecord* gr = new GenesisRecord();
            delete gr;
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

    }},

    {"set and get name from genesis record",[]{

        std::string name = "DUMMYNAME";
        GenesisRecord gr;

        gr.set_name(name);
        
        RCREQUIRE(name == gr.get_name());

    }},

    {"set and get the distribution list from genesis record",[]{

        std::vector<std::string> initial = {"FIRST","SECOND"};
        GenesisRecord gr;

        gr.set_distribution(initial);
       
        auto after = gr.get_distribution();
        for(unsigned int i = 0; i < after.size(); ++i){
            RCREQUIRE(initial[i] == after[i]);
        }
    }},
    
    {"get the type from genesis record",[]{

        GenesisRecord gr;
        RCREQUIRE(gr.get_type() == RecordType::Genesis);

    }},
    
    {"check validity of unsigned genesis record",[]{

        GenesisRecord gr;
        RCREQUIRE(!gr.is_valid());

    }},

    {"check validity of signed genesis record with no distribution list",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        
        GenesisRecord gr;
        private_key->sign(&gr);

        RCREQUIRE(!gr.is_valid());
        RCREQUIRE(!(gr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&gr));


        delete private_key;
        delete public_key;

    }},

    {"check validity of signed genesis record with distribution list",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        std::vector<std::string> dist = {"FIRST","SECOND"};
        
        GenesisRecord gr;
        gr.set_distribution(dist);
        private_key->sign(&gr);

        RCREQUIRE(!gr.is_valid());
        RCREQUIRE(!(gr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&gr));


        delete private_key;
        delete public_key;

    }},

    {"manually build a critical data string and compare",[]{

        std::vector<std::string> dist = {"FIRST","SECOND"};
        std::string key = "NOTAKEY";
        std::string name = "NAME";
        std::string previous = "PREVIOUS";

        GenesisRecord gr;

        gr.set_public_key(key);
        gr.set_previous(previous);
        gr.set_name(name);
        gr.set_distribution(dist);

        std::string data = gr.get_data();
        std::string manual;

        manual.append(key);
        manual.append(previous);
        manual.append(name);

        for(auto& d : dist){
            manual.append(d);
        }

        RCREQUIRE(manual == data);
    }},

    {"convert genesis record to/from string and compare",[]{

        GenesisRecord gr;
  
        std::string path = get_path("files/gold/test_gen_serialize.txt");
        std::ostringstream data;

        std::ifstream is(path);
        if(is.is_open()){

            boost::archive::text_iarchive archive(is);
            archive >> gr;

        }

        std::string gold = dump_file(path);
        std::string gray = gr.to_string();

        RCREQUIRE(gray == gold);

    }},

    {"sign a genesis record",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        GenesisRecord gr;

        try {
            private_key->sign(&gr);
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

        RCREQUIRE(!(gr.get_signature().empty()));
        RCREQUIRE(gr.get_public_key() == public_key->to_string());

        delete private_key;
        delete public_key;
    }},

    {"mine a genesis record without signing",[]{

        GenesisRecord gr;

        try {
            gr.mine();
        }
        catch(const std::invalid_argument& e){
            return;
        }

        RCTHROW("mining without signing did not fail");

    }},

    {"sign and mine a genesis record",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        std::vector<std::string> dist = {"FIRST","SECOND"};

        GenesisRecord gr;
        gr.set_distribution(dist);

        private_key->sign(&gr);
        gr.mine();

        RCREQUIRE(gr.hash() < HASH_MAX);
        RCREQUIRE(!(gr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&gr));
        RCREQUIRE(gr.is_valid());

        delete private_key;
        delete public_key;
    }},

    {"set and get the previous reference on a genesis record",[]{

        std::string hash = "NOTAHASH";
        GenesisRecord gr;

        gr.set_previous(hash);
        
        RCREQUIRE(gr.get_previous() == hash);

    }},
    
    {"set and get the public key on a genesis record",[]{

        std::string key = "NOTAPUBLICKEY";
        GenesisRecord gr;

        gr.set_public_key(key);
        
        RCREQUIRE(gr.get_public_key() == key);

    }},

    {"set and get the signature on a genesis record",[]{

        std::string sig = "NOTASIGNATURE";
        GenesisRecord gr;

        gr.set_signature(sig);
        
        RCREQUIRE(gr.get_signature() == sig);

    }},

    {"check nonce, timestamp and counter before/after mining",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        std::vector<std::string> dist = {"FIRST","SECOND"};

        GenesisRecord gr;
        gr.set_distribution(dist);

        RCREQUIRE(gr.get_nonce() == 0);
        RCREQUIRE(gr.get_timestamp() == 0);
        RCREQUIRE(gr.get_counter() == 0);

        private_key->sign(&gr);
        gr.mine();

        RCREQUIRE(gr.get_nonce() > 0);
        RCREQUIRE(gr.get_timestamp() > 0);
        RCREQUIRE(gr.get_counter() > 0);
        RCREQUIRE(gr.hash() < HASH_MAX);
        RCREQUIRE(!(gr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&gr));
        RCREQUIRE(gr.is_valid());

        delete private_key;
        delete public_key;
    }}

});
