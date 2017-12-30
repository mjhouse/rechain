#include "data.hpp"
#include "keys.hpp"

#include <string>

void Data::set_address( Address a ){ this->address = a; }
Address Data::get_address(){ return this->address; }

void Data::set_public_key( std::string k ){ this->public_key = k; }
std::string Data::get_public_key(){ return this->public_key; }

void Data::set_signature( std::string s ){ this->signature = s; }
<<<<<<< HEAD
std::string Data::get_signature(){ return this->signature; }
=======
std::string Data::get_signature(){ 
	return this->signature; }
>>>>>>> 87a9496026c2891bd964ab35e1298d3e1fc938ff

void Data::set_data_ref( std::string r ){ std::get<0>(this->address) = r; }
std::string Data::get_data_ref(){ return std::get<0>(this->address); }

void Data::set_block_ref( std::string r ){ std::get<1>(this->address) = r; }
std::string Data::get_block_ref(){ return std::get<1>(this->address); }

void Data::set_data_type( DataType d ){ std::get<2>(this->address) = d; }
DataType Data::get_data_type(){ return std::get<2>(this->address); }

std::string Data::to_string( bool b ){
	std::string data;
	data.append(std::get<0>(this->address));
	data.append(std::get<1>(this->address));
	data.append(std::to_string(std::get<2>(this->address)));
	data.append(this->public_key);

	if(b){ data.append(this->signature); }
	return data;
}

bool Data::verify(){
<<<<<<< HEAD
	// At a minimum, a Data object needs a public key and
	// signature to be valid
	if(!this->public_key.empty() && !this->signature.empty()){

		// Get the values from the Address
		std::string d_ref	= std::get<0>(this->address);
		std::string b_ref	= std::get<1>(this->address);
		DataType type 		= std::get<2>(this->address);

		// Check that it has a Data reference (or document hash) and
		// That it has a Block ref if it's a signature
		if( d_ref.empty() || (b_ref.empty() && type == DataType::Signature) ){
			return false;
		}

		// Load the public key and check the signature
		std::shared_ptr<PublicKey> key(PublicKey::load_string(this->public_key));
		return key->verify( shared_from_this() );
	}

=======
	if(!this->public_key.empty()){
		std::shared_ptr<PublicKey> key(PublicKey::load_string(this->public_key));
		return key->verify( shared_from_this() );
	}
>>>>>>> 87a9496026c2891bd964ab35e1298d3e1fc938ff
	return false;
}
