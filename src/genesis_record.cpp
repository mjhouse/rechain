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
#include "genesis_record.hpp"
#include "enums.hpp"
#include "keys.hpp"

// ----------------------------------------------------------------------------
// Name:
//      GenesisRecord::GenesisRecord
// Description:
//      Empty constructor 
// ----------------------------------------------------------------------------
GenesisRecord::GenesisRecord(){}

// ----------------------------------------------------------------------------
// Name:
//      GenesisRecord::~GenesisRecord
// Description:
//      Empty destructor 
// ----------------------------------------------------------------------------
GenesisRecord::~GenesisRecord(){}

// ----------------------------------------------------------------------------
// Name:
//      GenesisRecord::get_type
// Description:
//      Returns the type of the GenesisRecord (RecordType::Genesis)
// ----------------------------------------------------------------------------
int GenesisRecord::get_type(){
  return RecordType::Genesis;
}

// ----------------------------------------------------------------------------
// Name:
//      GenesisRecord::is_valid
// Description:
//      Checks GenesisRecord-specific values and that the Record is
//      signed and mined correctly.
// ----------------------------------------------------------------------------
bool GenesisRecord::is_valid(){
  bool valid = BaseRecord::is_valid();

  if(m_distribution.size() == 0){
    valid = false;
  }

  return valid;
}

// ----------------------------------------------------------------------------
// Name:
//      GenesisRecord::get_data
// Description:
//      Returns the concatenated data of the record
// ----------------------------------------------------------------------------
std::string GenesisRecord::get_data(){

    std::string data = BaseRecord::get_data();

    data.append(m_name);

    for(auto& id : m_distribution){
        data.append(id);
    }

    return data;
}

// ----------------------------------------------------------------------------
// Name:
//      GenesisRecord::to_string
// Description:
//      Serializes the GenesisRecord into a string
// ----------------------------------------------------------------------------
std::string GenesisRecord::to_string(){

  // serialize the record
  std::ostringstream data;

  {
      boost::archive::text_oarchive archive{data};
      archive << *this;
  }

  return data.str();

}

BOOST_CLASS_EXPORT_IMPLEMENT(GenesisRecord)
