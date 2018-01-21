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

/** A single record that can be stored
	within a Block on the BlockChain
*/
class Record {
	private:
		std::string _reference;		/**< A reference to a document or another Record */
		std::string _block;		/**< Block hash, only used if this is a Signature record*/
		std::string _public_key;	/**< The public key of the owner */
		std::string _signature;		/**< The signature */

		float _trust;			/**< The trust of the Record (only used by Signatures) */

	public:
		/** Empty constructor */
		Record();

		/** Construct from a reference and block hash
			\param r A file hash or reference to another Record
			\param b An optional Block hash
		*/
		Record( std::string r, std::string b = "" );

		/** Construct a Publication Record from a file
			\param r An open ifstream object
		*/
		Record( std::ifstream& r );

		/** Empty destructor */
		~Record();

		/** Update or get the reference
			\param s The reference to use
			\returns The value of _reference
		*/
		std::string& reference( std::string s = "" );

		/** Update or get the Block hash
			\param b The Block hash to use
			\returns The value of _block
		*/
		std::string& block( std::string b = "" );

		/** Update or get the public key
			\param k The public key to use
			\returns The value of _public_key
		*/
		std::string& public_key( std::string k = "" );

		/** Update or get the signature
			\param s The signature to use
			\returns The value of _signature
		*/
		std::string& signature( std::string s = "" );

		/** Update or get the trust
			\param t The trust value to use
			\returns The value of _trust
		*/
		float& trust( float t = 0.0 ); 

		/** Get the DataType of this Record
			\returns The DataType of this Record
		*/
		DataType type();

		/** Get Record as a single string
			\param b False returns a subset of values
			\returns Data in the Record as a string
		*/
		std::string string( bool b = false );

		/** Check if Record is internally valid
			\returns True if Record is valid
		*/
		bool valid();

		/** Serialize Record to an archive/file
			\param ar The archive to serialize to
		*/
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
