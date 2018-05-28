#include <iostream>
#include <fstream>
#include <memory>

#include <boost/archive/text_iarchive.hpp>
#include "test-framework.hpp"

#include "publication_record.hpp"
#include "signature_record.hpp"
#include "enums.hpp"
#include "keys.hpp"

test_set signature_tests("tests for signature records",{

    {"call publication record default constructor",[]{

        try {
            SignatureRecord sr;
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

    }},

    {"construct a signature from a given hash",[]{
       
        std::string hash = generate_hash();
        SignatureRecord sr(hash);

        RCREQUIRE(sr.get_record_hash() == hash);

    }},

    {"test the signature record destructor",[]{

        try {
            SignatureRecord* sr = new SignatureRecord();
            delete sr;
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

    }},

    {"get and set hash on a signature record",[]{

        std::string hash = generate_hash();
        SignatureRecord sr;
        sr.set_record_hash(hash);

        RCREQUIRE(sr.get_record_hash() == hash);

    }},

    {"set a reference on a publication record",[]{

        std::string hash = generate_hash();

        PublicationRecord pr;
        pr.set_reference(hash);
        
        RCREQUIRE(hash == pr.get_reference());

    }},

    {"get the type from signature record",[]{

        SignatureRecord sr;
        RCREQUIRE(sr.get_type() == RecordType::Signature);

    }},
    
    {"check validity of signature record with no record hash",[]{

        SignatureRecord sr;
        RCREQUIRE(!sr.is_valid());

    }},

    {"check validity of unsigned signature record",[]{

        std::string hash = generate_hash();
        SignatureRecord sr(hash);

        RCREQUIRE(!sr.is_valid());
        RCREQUIRE(sr.get_record_hash() == hash);

    }},

    {"check validity of signed signature record with no record hash",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        
        PublicationRecord sr;
        private_key->sign(&sr);

        RCREQUIRE(!sr.is_valid());
        RCREQUIRE(!(sr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&sr));


        delete private_key;
        delete public_key;

    }},

    {"check validity of signed signature record with record_hash",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
       
        std::string hash = generate_hash();
        SignatureRecord sr(hash);
        private_key->sign(&sr);

        RCREQUIRE(!sr.is_valid());
        RCREQUIRE(!(sr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&sr));


        delete public_key;
        delete private_key;

    }},

    {"manually build a critical data string and compare",[]{

        std::string hash = generate_hash();
        std::string prev = generate_hash();

        SignatureRecord sr(hash);
        sr.set_previous(prev);

        std::string data = sr.get_data();

        std::string manual;
        manual.append(sr.get_public_key());
        manual.append(sr.get_previous());
        manual.append(sr.get_record_hash());

        RCREQUIRE(manual == data);
        RCREQUIRE(sr.get_previous() == prev);
        RCREQUIRE(sr.get_record_hash() == hash);
    }},

    {"convert signature record to/from string and compare",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));

        SignatureRecord sr("NOTAHASH");
        sr.set_previous("NOTAHASH");

        private_key->sign(&sr);
        delete private_key;

        //std::ofstream ofs(get_path("files/gold/test_sig_serialize.txt"));
        //ofs << sr.to_string();
        //return;
  
        std::string path = get_path("files/gold/test_sig_serialize.txt");
        std::ostringstream data;

        std::ifstream is(path);
        if(is.is_open()){

            boost::archive::text_iarchive archive(is);
            archive >> sr;

        }

        std::string gold = dump_file(path);
        std::string gray = sr.to_string();

        RCREQUIRE(gray == gold);

    }},

    {"sign a signature record",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        SignatureRecord sr;

        try {
            private_key->sign(&sr);
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

        RCREQUIRE(!(sr.get_signature().empty()));
        RCREQUIRE(sr.get_public_key() == public_key->to_string());

        delete private_key;
        delete public_key;
    }},

    {"mine a signature record without signing",[]{

        SignatureRecord sr(generate_hash());

        try {
            sr.mine();
        }
        catch(const std::invalid_argument& e){
            return;
        }

        RCTHROW("mining without signing did not fail");

    }},

    {"sign and mine a signature record",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();

        SignatureRecord sr(generate_hash());

        private_key->sign(&sr);
        sr.mine();

        RCREQUIRE(sr.hash() < HASH_MAX);
        RCREQUIRE(!(sr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&sr));
        RCREQUIRE(!(sr.get_record_hash().empty()));
        RCREQUIRE(sr.is_valid());

        delete private_key;
        delete public_key;
    }},

    {"set and get the previous reference on a signature record",[]{

        std::string hash = generate_hash();
        SignatureRecord sr;

        sr.set_previous(hash);
        
        RCREQUIRE(sr.get_previous() == hash);

    }},
    
    {"set and get the public key on a signature record",[]{

        std::string key = "NOTAPUBLICKEY";
        SignatureRecord sr;

        sr.set_public_key(key);
        
        RCREQUIRE(sr.get_public_key() == key);

    }},

    {"set and get the signature on a signature record",[]{

        std::string sig = "NOTASIGNATURE";
        SignatureRecord sr;

        sr.set_signature(sig);
        
        RCREQUIRE(sr.get_signature() == sig);

    }},

    {"check nonce, timestamp and counter before/after mining",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();

        SignatureRecord sr(generate_hash());

        RCREQUIRE(sr.get_nonce() == 0);
        RCREQUIRE(sr.get_timestamp() == 0);
        RCREQUIRE(sr.get_counter() == 0);

        private_key->sign(&sr);
        sr.mine();

        RCREQUIRE(sr.get_nonce() > 0);
        RCREQUIRE(sr.get_timestamp() > 0);
        RCREQUIRE(sr.get_counter() > 0);
        RCREQUIRE(sr.hash() < HASH_MAX);
        RCREQUIRE(!(sr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&sr));
        RCREQUIRE(!(sr.get_record_hash().empty()));
        RCREQUIRE(sr.is_valid());

        delete private_key;
        delete public_key;
    }}

});
