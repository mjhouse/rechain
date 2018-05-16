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
 * Contact: mjhouse@protonmail.com
 *
*/

// system includes
#include <sstream>

// dependency includes
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// local includes
#include "signature_record.hpp"
#include "enums.hpp"
#include "keys.hpp"

// ----------------------------------------------------------------------------
// Name:
//      SignatureRecord::SignatureRecord
// Description:
//      Empty constructor 
// ----------------------------------------------------------------------------
SignatureRecord::SignatureRecord(){}

// ----------------------------------------------------------------------------
// Name:
//      SignatureRecord::SignatureRecord
// Description:
//      Construct a new signature from a hash 
// ----------------------------------------------------------------------------
SignatureRecord::SignatureRecord( std::string t_hash ){
    m_record_hash = t_hash;
}

// ----------------------------------------------------------------------------
// Name:
//      SignatureRecord::~SignatureRecord
// Description:
//      Empty destructor 
// ----------------------------------------------------------------------------
SignatureRecord::~SignatureRecord(){}

// ----------------------------------------------------------------------------
// Name:
//      SignatureRecord::get_type
// Description:
//      Returns the type of the SignatureRecord (RecordType::Signature)
// ----------------------------------------------------------------------------
int SignatureRecord::get_type(){
  return RecordType::Signature;
}

// ----------------------------------------------------------------------------
// Name:
//      SignatureRecord::is_valid
// Description:
//      Checks SignatureRecord-specific values and that the Record is
//      signed and mined correctly.
// ----------------------------------------------------------------------------
bool SignatureRecord::is_valid(){
  bool valid = BaseRecord::is_valid();

  // check if the signature has a reference
  if(m_record_hash.empty()){
      valid = false;
  }

  return valid;
}

// ----------------------------------------------------------------------------
// Name:
//      SignatureRecord::get_data
// Description:
//      Returns the concatenated data of the record
// ----------------------------------------------------------------------------
std::string SignatureRecord::get_data(){

    std::string data = BaseRecord::get_data();

    data.append(m_record_hash);

    return data;
}

// ----------------------------------------------------------------------------
// Name:
//      SignatureRecord::to_string
// Description:
//      Serializes the SignatureRecord into a string
// ----------------------------------------------------------------------------
std::string SignatureRecord::to_string(){

  // serialize the record
  std::ostringstream data;

  {
      boost::archive::text_oarchive archive{data};
      archive << *this;
  }

  return data.str();

}

