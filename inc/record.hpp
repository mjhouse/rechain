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

/**	\file	record.hpp
	\brief	Defines a Record object that holds publishing
		information or a signature for another
		published Record object.
*/

#ifndef _RECHAIN_RECORD_HPP_
#define _RECHAIN_RECORD_HPP_

// system includes
#include <string>
#include "cereal/cereal.hpp"

class PrivateKey;

/** An enum type for different flavors of 
	Record objects.
*/
enum DataType { Publication, Signature };

class Record {
	private:
		std::string _reference;
		std::string _block;
		std::string _public_key;
		std::string _signature;

		float _trust;

	public:
		Record();

		Record( std::string r, std::string b = "" );

		Record( std::ifstream& r );

		~Record();

		std::string& reference( std::string s = "" );

		std::string& block( std::string b = "" );
		
		std::string& public_key( std::string k = "" );

		std::string& signature( std::string s = "" );

		float& trust( float t = 0.0 ); 

		DataType type();

		std::string string( bool b = false );

		bool valid();

		template <class Archive>
		void serialize( Archive& ar ){
			ar(
				CEREAL_NVP(_reference),
				CEREAL_NVP(_block),
				CEREAL_NVP(_public_key),
				CEREAL_NVP(_signature),
				CEREAL_NVP(_trust)
			);
		}

};

#endif
