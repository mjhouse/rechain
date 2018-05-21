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

/** \file	keys.hpp
	\brief	Defines the Key classes used to manage,
			load, save and generate a public and
			private pair of RSA256 keys.
*/

// system includes
#include <fstream>			    // File I/O
#include <iostream>
#include <stdexcept>

// dependency includes
#include <cryptopp/osrng.h>		// For AutoSeededRandomPool
#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace

// local includes
#include "base_record.hpp"		// Data objects

#ifndef _RECHAIN_KEYS_HPP_
#define _RECHAIN_KEYS_HPP_

class PrivateKey;
class PublicKey;

/** The templated Key class acts as a base class for both
	PrivateKey and PublicKey.
*/
template <typename T, typename K>
class Key {
	protected:
		T key;	/**< The CryptoPP::*Key to use*/

		Key(){}

	public:
		
		/** Create an empty Key
			\returns A pointer to the Key
		*/
		static K* empty(){
			return new K();
		};

		/** Create a new Key from a string
			\param s A string representation of a key
			\returns A pointer to the Key
		*/
		static K* load_string( std::string s ){
			K* k = new K();
			k->from_string(s);
			
			if(!k->valid()) throw std::invalid_argument("Given key isn't valid");
			
			return k;
		};

		/** Create a new Key from a file
			\param fn A path to the file to use
			\returns A pointer to the Key
		*/
		static K* load_file( std::string fn ){
			K* k = new K();
			k->load(fn);

			if(!k->valid()) throw std::invalid_argument("Given key isn't valid");

			return k;
		};

		/** Get the key as a CryptoPP object
			\returns The key
		*/
		T get_key(){ return this->key; }

		/** Set a CryptoPP object as key
			\param k The key to use
		*/
		void set_key( T k ){ this->key = k; }

		/** Convert the key to a hex encoded string
			\returns The hex encoded string
		*/
		std::string to_string(){
			std::string s;
			this->key.Save(CryptoPP::HexEncoder( new CryptoPP::StringSink(s)).Ref());
			return s;
		}

		/** Build a CryptoPP private or public key from
			a hex encoded string
			\param k The string to build
		*/
		void from_string( std::string k ){
			this->key.Load(CryptoPP::StringSource(k, true, new CryptoPP::HexDecoder()).Ref());
		}


		/** Verify that the key is valid
			\returns True if valid, false otherwise
		*/
		bool valid(){
			CryptoPP::AutoSeededRandomPool rng;
			return this->key.Validate(rng,1);
		}

		/** Load a key from a file
			\param fn The path to the file
			\returns True if file was loaded
		*/
		bool load( std::string fn ){
			std::ifstream file(fn);
			if( file.is_open() ){
				this->from_string( std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()) );
				return true;
			}
			return false;
		}

		/** Save the key to a file
			\param fn The file to save to
			\returns True if key was saved
		*/
		bool save( std::string fn ){
			std::ofstream file(fn);
			if( file.is_open() ){
				file << this->to_string();
				return true;
			}
			return false;
		}
};

/** The PrivateKey class inherits from the templated
	'Key' base class and adds private-key-specific methods.
*/
class PrivateKey : public Key<CryptoPP::RSA::PrivateKey,PrivateKey> {
	public:
		/** \brief Empty constructor */
		PrivateKey(){}

		/** \brief Copy constructor 
            \param t_key The key to copy
        */
		PrivateKey( PrivateKey* t_key ){
            key = t_key->key;
        }

		/** \brief Generate a new key */
		void generate();

        /** \brief Get a public key
            \returns A pointer to a public key
        */
        PublicKey* get_public();

		/** \brief Sign a given Record
			\param t_record A pointer to the record to sign
		*/
		void sign( BaseRecord* t_record );

		/** \brief Sign a given Record
			\param t_record A pointer to the record to sign
		*/
		void sign( std::shared_ptr<BaseRecord> t_record );

};

/** The PublicKey class inherits from the templated
	'Key' base class and adds public-key-specific methods.
*/
class PublicKey: public Key<CryptoPP::RSA::PublicKey,PublicKey> {
	public:
		/** \brief Empty constructor */
		PublicKey(){}

		/** \brief Copy constructor
            \param t_key The key to copy
        */
		PublicKey( PublicKey* t_key ){
            key = t_key->key;
        }

		/** \brief Generate a new PublicKey from a PrivateKey
			\param t_key The PrivateKey to generate from
		*/
		void generate( PrivateKey* t_key );

		/** \brief Verify a Record to ensure that the
		 	       signature attached to it is correct.
			\param t_record A pointer to the Record to verify
			\returns True if the Record is signed correctly
		*/
		bool verify( BaseRecord* t_record );
};

#endif
