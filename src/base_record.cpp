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
 * ME
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: mjhouse@protonmail.com
 *
*/

// system includes
#include <string>
#include <sstream>
#include <chrono>

// dependency includes
#include <cryptopp/files.h>     // for FileSou
#include <cryptopp/osrng.h>     // for the AutoSeededRandomPool
#include <cryptopp/integer.h>   // for Integer data type
#include <cryptopp/hex.h>       // for the HexEncoder

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// local includes
#include "base_record.hpp"
#include "enums.hpp"
#include "keys.hpp"

// ----------------------------------------------------------------------------
// Name:
//      BaseRecord::BaseRecord
// Description:
//      Constructor that inits default values
// ----------------------------------------------------------------------------
BaseRecord::BaseRecord()
    : m_type(RecordType::Base), m_nonce(0), m_timestamp(0), m_counter(0) {}

// ----------------------------------------------------------------------------
// Name:
//      BaseRecord::hash
// Description:
//      Hash the BaseRecord with current values (for nonce, count etc.)
// ----------------------------------------------------------------------------
std::string BaseRecord::hash(){

    // get the record as a string
	std::string data = to_string();

	CryptoPP::SHA256 hasher;
	std::string hash;

  // hash, hex encode and save to 'hash'
	CryptoPP::StringSource ss(data,true,
		new CryptoPP::HashFilter(hasher,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(hash))));

	// return hash
	return hash;

}

// ----------------------------------------------------------------------------
// Name:
//      BaseRecord::mine
// Description:
//      Mine the BaseRecord
// ----------------------------------------------------------------------------
std::string BaseRecord::mine(){

    if(m_signature.empty()){
        throw std::invalid_argument("record has not been signed");
    }

	// mine the record until it has a valid hash
	while(hash() > HASH_MAX){

		// update the random nonce
		CryptoPP::AutoSeededRandomPool rng;
		m_nonce = CryptoPP::Integer(rng,
			CryptoPP::Integer(1),
			CryptoPP::Integer(LONG_MAX)).ConvertToLong();

		// update the timestamp
		auto e = std::chrono::system_clock::now().time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(e).count();
		m_timestamp = (long)seconds;

        // update the counter
        m_counter++;

	}

	return hash();

}

// ----------------------------------------------------------------------------
// Name:
//      BaseRecord::is_valid
// Description:
//      Checks BaseRecord-specific values and that the Record is
//      signed and mined correctly.
// ----------------------------------------------------------------------------
bool BaseRecord::is_valid(){
  bool valid = true;

  try {

      // if the public key is bad this will throw
      std::shared_ptr<PublicKey> key(PublicKey::load_string(m_public_key));

      // check if the signature is valid
      if(!key->verify( this )){
          valid = false;
      }

      // check if the Record has been mined
      if(hash() > HASH_MAX){
          valid = false;
      }

  } catch (const CryptoPP::InvalidArgument& e){
      valid = false;
  }

  return valid;
}


// ----------------------------------------------------------------------------
// Name:
//      BaseRecord::get_data
// Description:
//      Get the concatenated values of the record without the signature
// ----------------------------------------------------------------------------
std::string BaseRecord::get_data(){
    std::string data;

    data.append(m_public_key);
    data.append(m_previous);

    return data;
}

// ----------------------------------------------------------------------------
// Name:
//      BaseRecord::get_type
// Description:
//      Get the type of this record      
// ----------------------------------------------------------------------------
RecordType BaseRecord::get_type(){
    return m_type;
}

