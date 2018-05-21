#include <iostream>
#include <fstream>
#include <memory>

#include <boost/archive/text_iarchive.hpp>
#include "test-framework.hpp"

#include "publication_record.hpp"
#include "enums.hpp"
#include "keys.hpp"

test_set publication_tests("tests for publication records",{

    {"call publication record default constructor",[]{

        try {
            PublicationRecord pr;
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

    }},

    {"call path constructor with bad path",[]{
        
        try {
            PublicationRecord pr("bad path");
        }
        catch (const std::exception& e){
            return;
        }

        RCTHROW("constructor with bad path didn't fail");

    }},

    {"call path constructor with good path",[]{

        try {
            PublicationRecord pr(get_path("files/general/test_publication.txt"));
        }
        catch (const std::exception& e){
            RCTHROW(e.what());
        }

    }},

    {"test the publication record destructor",[]{

        try {
            PublicationRecord* pr = new PublicationRecord();
            delete pr;
        }
        catch(const std::exception& e){
            RCTHROW(e.what());
        }

    }},

    {"get reference from publication record",[]{

        std::string hash = "93D6AAACFF0F5257DAFAC376247EC8B7929854CDD9126F6FF4D38CDF96866A08";
        PublicationRecord pr(get_path("files/general/test_publication.txt"));
        
        RCREQUIRE(hash == pr.get_reference());

    }},

    {"set a reference on a publication record",[]{

        std::string hash = "NOTAHASH";

        PublicationRecord pr;
        pr.set_reference(hash);
        
        RCREQUIRE(hash == pr.get_reference());

    }},

    {"get the type from publication record",[]{

        PublicationRecord pr(get_path("files/general/test_publication.txt"));
        RCREQUIRE(pr.get_type() == RecordType::Publication);

    }},
    
    {"check validity of publication record with no reference",[]{

        PublicationRecord pr;
        RCREQUIRE(!pr.is_valid());

    }},

    {"check validity of unsigned publication record",[]{

        PublicationRecord pr(get_path("files/general/test_publication.txt"));
        RCREQUIRE(!pr.is_valid());

    }},

    {"check validity of signed publication record with no reference",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        
        PublicationRecord pr;
        private_key->sign(&pr);

        RCREQUIRE(!pr.is_valid());
        RCREQUIRE(!(pr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&pr));


        delete private_key;
        delete public_key;

    }},

    {"check validity of signed publication record with reference",[]{

        PrivateKey* private_key = PrivateKey::load_file(get_path("keys/rsa.private"));
        PublicKey* public_key   = private_key->get_public();
        
        PublicationRecord pr(get_path("files/general/test_publication.txt"));
        private_key->sign(&pr);

        RCREQUIRE(!pr.is_valid());
        RCREQUIRE(!(pr.get_signature().empty()));
        RCREQUIRE(public_key->verify(&pr));


        delete public_key;
        delete private_key;

    }},

    {"manually build a critical data string and compare",[]{

        PublicationRecord pr(get_path("files/general/test_publication.txt"));
        pr.set_previous("NOTAHASH");

        std::string data = pr.get_data();
        std::string manual;

        manual.append(pr.get_public_key());
        manual.append(pr.get_previous());
        manual.append(pr.get_reference());

        RCREQUIRE(manual == data);
    }},

    {"convert publication record to/from string and compare",[]{

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
