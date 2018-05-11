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
#include "publication_record.hpp"
#include "enums.hpp"
#include "keys.hpp"

// ----------------------------------------------------------------------------
// Name:
//      PublicationRecord::PublicationRecord
// Description:
//      Empty constructor 
// ----------------------------------------------------------------------------
PublicationRecord::PublicationRecord(){};

// ----------------------------------------------------------------------------
// Name:
//      PublicationRecord::~PublicationRecord
// Description:
//      Empty destructor 
// ----------------------------------------------------------------------------
PublicationRecord::~PublicationRecord(){};

// ----------------------------------------------------------------------------
// Name:
//      PublicationRecord::get_type
// Description:
//      Returns the type of the PublicationRecord (RecordType::Genesis)
// ----------------------------------------------------------------------------
int PublicationRecord::get_type(){
  return RecordType::Publication;
}

// ----------------------------------------------------------------------------
// Name:
//      PublicationRecord::is_valid
// Description:
//      Checks PublicationRecord-specific values and that the Record is
//      signed and mined correctly.
// ----------------------------------------------------------------------------
bool PublicationRecord::is_valid(){
  bool valid = BaseRecord::is_valid();

  // check if the publication has a reference
  if(m_reference.empty()){
      valid = false;
  }

  return valid;
}

// ----------------------------------------------------------------------------
// Name:
//      PublicationRecord::to_string
// Description:
//      Serializes the PublicationRecord into a string
// ----------------------------------------------------------------------------
std::string PublicationRecord::to_string(){

  // serialize the record
  std::ostringstream data;

  {
      boost::archive::text_oarchive archive{data};
      archive << *this;
  }

  return data.str();

}

