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

/** \file	data.hpp
	\brief	Defines a Data object that holds publishing
			information or a signature for another
			published Data object.
*/
#ifndef _DATA_HPP_
#define _DATA_HPP_

// system includes
#include <tuple>
#include <memory>
#include <string>

// dependency includes
#include "cereal/types/tuple.hpp"

class PrivateKey;

/** An enumerated type for defining different
	kinds of Address references.
*/
enum DataType { Publication, Signature };

/** Typedef for conveniance */
typedef std::tuple<std::string,std::string,DataType> Address;

/** The Data class maintains a reference internally to another
	Data object (if this is a signature) or to a hash for a
	document (if the Data is a publication).
*/
class Data {
	private:
		Address address;		/**< The reference to publish*/
		std::string public_key;		/**< The public key to match the signature */
		std::string signature;		/**< The signature for this data */

		float trust;			/**< Recorded trust for Signature records*/

	public:
		
		/** Empty constructor */
		Data(){}

		/** Constructor that initializes the Address.
		    \param a The Address to use
		*/
		Data( Address a ) : address(a) , trust(0) {}

		/** Constructor that initializes the Address and signs
		    \param a The Address to use
		    \param p The PrivateKey to sign with
		*/
		Data( Address a, PrivateKey* p );

		/** Empty destructor */
		~Data(){}

		/** Set the trust value
		    \param t The new value for trust
		*/
		void set_trust( float t );

		/** Get the current trust value
		    \returns The current value of trust
		*/
		float get_trust();

		/**	Set the Address for the Data object
			\param a An Address to use
		*/
		void set_address( Address a );

		/** Get the Address from the Data object
			\returns The Address currently used
		*/
		Address get_address();

		/** Set the public key for the Data object
			\param k The public key to use (should match signature)
		*/
		void set_public_key( std::string k );

		/** Get the public key from the Data object
			\returns The public key as a hex encoded string
		*/
		std::string get_public_key();

		/** Set the signature on the Data object
			\param s The signature to hold
		*/
		void set_signature( std::string s );

		/** Get the signature from the Data object
			\returns The current signature
		*/
		std::string get_signature();

		/** Set the Data reference in the Address
			\param r Either the signature of a published Data block
					 or the hash of a document to publish
		*/
		void set_data_ref( std::string r );

		/** Get the Data reference from the Address
			\returns Either a signature for a publish Data object
					 or the hash of a document.
		*/
		std::string get_data_ref();

		/** Set the block reference in the Address
			\param r A hash of a referenced Block
		*/
		void set_block_ref( std::string r );

		/** Get the Block reference from the Address
			\returns A hash for a published Block or empty string
		*/
		std::string get_block_ref();

		/** Set the data type of the Address
			\param d Either 'Signature' or 'Publication'
		*/
		void set_data_type( DataType d );

		/** Get the data type of the Address
			\returns Either 'Signature' or 'Publication'
		*/
		DataType get_data_type();

		/** Get the held data and the counter/nonce/timestamp
			as a string for hashing.
			\param b True if the signature should be included
			\returns A string of all held data
		*/
		std::string to_string( bool b = false );

		/** Check if this Data object is valid
			\returns True if block is signed and valid
		*/
		bool verify();

		/** Serialize/Unserialize this Data object
		    \param ar The archive to serialize to or from
		*/
		template <class Archive>
		void serialize( Archive& ar ){
			ar(
				CEREAL_NVP(address),
				CEREAL_NVP(public_key),
				CEREAL_NVP(signature),
				CEREAL_NVP(trust)
			);
		}
};

#endif
