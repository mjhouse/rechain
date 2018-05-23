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

        PublicationRecord pr;
  
        std::string path = get_path("files/gold/test_pub_serialize.txt");
        std::ostringstream data;

        std::ifstream is(path);
        if(is.is_open()){

            boost::archive::text_iarchive archive(is);
            archive >> pr;

        }

        std::string gold = dump_file(path);
        std::string gray = pr.to_string();

        RCREQUIRE(gray == gold);

    }},

    {"sign a publication record",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        PublicationRecord pr;

        try {
            private_key->sign(&pr);
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

        RCREQUIRE(!(pr.get_signature().empty()));
        RCREQUIRE(pr.get_public_key() == public_key->to_string());

        delete private_key;
        delete public_key;
    }},

    {"mine a publication record without signing",[]{

        PublicationRecord pr(get_path("files/general/test_publication.txt"));

        try {
            pr.mine();
        }
        catch(const std::invalid_argument& e){
            return;
        }

        RCTHROW("mining without signing did not fail");

    }},

    {"sign and mine a publication record",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();

        PublicationRecord pr(get_path("files/general/test_publication.txt"));

        private_key->sign(&pr);
        pr.mine();

        RCREQUIRE(pr.hash() < HASH_MAX);
        RCREQUIRE(!(pr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&pr));
        RCREQUIRE(!(pr.get_reference().empty()));
        RCREQUIRE(pr.is_valid());

        delete private_key;
        delete public_key;
    }},

    {"set and get the previous reference on a publication record",[]{

        std::string hash = "NOTAHASH";
        PublicationRecord pr;

        pr.set_previous(hash);
        
        RCREQUIRE(pr.get_previous() == hash);

    }},
    
    {"set and get the public key on a publication record",[]{

        std::string key = "NOTAPUBLICKEY";
        PublicationRecord pr;

        pr.set_public_key(key);
        
        RCREQUIRE(pr.get_public_key() == key);

    }},

    {"set and get the signature on a publication record",[]{

        std::string sig = "NOTASIGNATURE";
        PublicationRecord pr;

        pr.set_signature(sig);
        
        RCREQUIRE(pr.get_signature() == sig);

    }},

    {"check nonce, timestamp and counter before/after mining",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();

        PublicationRecord pr(get_path("files/general/test_publication.txt"));

        RCREQUIRE(pr.get_nonce() == 0);
        RCREQUIRE(pr.get_timestamp() == 0);
        RCREQUIRE(pr.get_counter() == 0);

        private_key->sign(&pr);
        pr.mine();

        RCREQUIRE(pr.get_nonce() > 0);
        RCREQUIRE(pr.get_timestamp() > 0);
        RCREQUIRE(pr.get_counter() > 0);

        delete private_key;
        delete public_key;
    }}

});
