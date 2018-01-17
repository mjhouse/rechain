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

#include "record.hpp"
#include "keys.hpp"
#include "logger.hpp"

#include <string>
#include <sstream>


Record::Record() : _trust(0.0f) {}

Record::Record( std::string r, std::string b ) : _reference(r), _block(b), _trust(0.0f) {}

Record::Record( std::ifstream& r ) : _trust(0.0f) {
	
	CryptoPP::SHA256 hasher;
	
	std::string data = [&r]{
		std::ostringstream ss{};
		ss << r.rdbuf();
		return ss.str(); }();

	CryptoPP::StringSource ss(data,true,
		new CryptoPP::HashFilter(hasher,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(_reference)))); 
}

Record::~Record(){}

std::string& Record::reference( std::string s ){
	if(!s.empty()) _reference = s;
	return _reference;
}

std::string& Record::block( std::string b ){
	if(!b.empty()) _block = b;
	return _block;
}

std::string& Record::public_key( std::string k ){
	if(!k.empty()) _public_key = k;
	return _public_key;
}

std::string& Record::signature( std::string s ){
	if(!s.empty()) _signature = s;
	return _signature;
}

float& Record::trust( float t ){ 
	if(t) _trust = t;
	return _trust;
}

DataType Record::type(){
	if(_block.empty()) return DataType::Publication;
	return DataType::Signature;
}

std::string Record::string( bool b ){
	std::string data;
	
	data.append(_reference);
	data.append(_block);
	data.append(_public_key);

	if(b){
		data.append(_signature);
		data.append(std::to_string(_trust));
	}
	return data;
}

bool Record::valid(){
	if(!_public_key.empty() && !_signature.empty()){
		if(_reference.empty()) return false;
		if(_block.empty() && type() != DataType::Publication) return false;

		std::shared_ptr<PublicKey> key(PublicKey::load_string(_public_key));
		return key->verify( *this );
	}

	return false;
}

