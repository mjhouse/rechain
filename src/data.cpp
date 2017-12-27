#include "data.hpp"
#include "keys.hpp"
#include <sstream>

void Data::set_address( Address a ){ this->address = a; }
Address Data::get_address(){ return this->address; }

void Data::set_public_key( std::string k ){ this->public_key = k; }
std::string Data::get_public_key(){ return this->public_key; }

void Data::set_signature( std::string s ){ this->signature = s; }
std::string Data::get_signature(){ return this->signature; }

void Data::set_data_ref( std::string r ){ std::get<0>(this->address) = r; }
std::string Data::get_data_ref(){ return std::get<0>(this->address); }

void Data::set_block_ref( std::string r ){ std::get<1>(this->address) = r; }
std::string Data::get_block_ref(){ return std::get<1>(this->address); }

void Data::set_data_type( DataType d ){ std::get<2>(this->address) = d; }
DataType Data::get_data_type(){ return std::get<2>(this->address); }

std::string Data::to_string( bool b ){
	std::stringstream stream;
	stream	<< std::get<0>(this->address)
			<< std::get<1>(this->address)
			<< std::get<2>(this->address)
			<< this->public_key;

	if(b) stream << this->signature;
	return stream.str();
}

bool Data::verify(){
	if(!this->public_key.empty()){
		PublicKey key;
		key.from_string( this->public_key );

		return key.verify( shared_from_this() );
	}
	return false;
}
