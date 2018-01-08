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
 * Contact: michaelhouse@gmx.us
 *
*/

#include "data.hpp"
#include "keys.hpp"

#include <string>

/** Constructor that initializes the Address and signs
*/
Data::Data( Address a, PrivateKey* p ) : address(a) , trust(0) {
	p->sign( this );
}

void Data::set_trust( float t ){ this->trust = t; }
float Data::get_trust(){ return this->trust; }

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
	std::string data;
	data.append(std::get<0>(this->address));
	data.append(std::get<1>(this->address));
	data.append(std::to_string(std::get<2>(this->address)));
	data.append(this->public_key);

	if(b){
		data.append(this->signature);
		data.append(std::to_string(this->trust));
	}
	return data;
}

bool Data::verify(){
	// At a minimum, a Data object needs a public key and
	// signature to be valid
	if(!this->public_key.empty() && !this->signature.empty()){

		// Get the values from the Address
		std::string d_ref	= std::get<0>(this->address);
		std::string b_ref	= std::get<1>(this->address);
		DataType type 		= std::get<2>(this->address);

		// There has to be a data reference
		if(d_ref.empty()) return false;

		// If there is no block reference, type must be Publication
		if(b_ref.empty() && type != DataType::Publication) return false;

		// Load the public key and check the signature
		std::shared_ptr<PublicKey> key(PublicKey::load_string(this->public_key));
		return key->verify( this );
	}

	return false;
}
