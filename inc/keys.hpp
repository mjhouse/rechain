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

#ifndef _KEYS_HPP_
#define _KEYS_HPP_

#include <fstream>				// File I/O
#include <iostream>
#include <memory>

#include <cryptopp/hex.h>		// For HexEncoder/HexDecoder
#include <cryptopp/rsa.h>		// For RSA:: namespace

#include "data.hpp"				// Data objects
#include "error.hpp"

/** The templated Key class acts as a base class for both
	PrivateKey and PublicKey.
*/
template <typename T>
class Key {
	protected:
		T key;	/**< The CryptoPP::*Key to use*/

		Key(){}

	public:
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
class PrivateKey : public Key<CryptoPP::RSA::PrivateKey>, public std::enable_shared_from_this<PrivateKey> {
	private:

		/** Empty constructor */
		PrivateKey(){}

	public:
		/** Create an empty PrivateKey
			\returns A pointer to the PrivateKey
		*/
		static PrivateKey* empty(){
			return new PrivateKey();
		};

		/** Create a new PrivateKey from a string
			\param s A string representation of a public key
			\returns A pointer to the PrivateKey
		*/
		static PrivateKey* load_string( std::string s ){
			PrivateKey* k = new PrivateKey();
			k->from_string(s);
			return k;
		};

		/** Create a new PrivateKey from a file
			\param fn A path to the file to use
			\returns A pointer to the PrivateKey
		*/
		static PrivateKey* load_file( std::string fn ){
			PrivateKey* k = new PrivateKey();
			if(!k->load(fn)){ throw rechain::LoadFailure(fn); }
			return k;
		};

		/** Generate a new key */
		void generate();

		/** Sign a given Data block
			\param data A shared_ptr to the Data object to sign
			\returns True if the Data object was signed
		*/
		bool sign( std::shared_ptr<Data> data );
};

/** The PublicKey class inherits from the templated
	'Key' base class and adds public-key-specific methods.
*/
class PublicKey: public Key<CryptoPP::RSA::PublicKey>, public std::enable_shared_from_this<PublicKey> {
	private:

		/** Empty constructor */
		PublicKey(){}

	public:

		/** Create an empty PublicKey
			\returns A pointer to the PublicKey
		*/
		static PublicKey* empty(){
			return new PublicKey();
		};

		/** Create a new PublicKey from a string
			\param s A string representation of a public key
			\returns A pointer to the PublicKey
		*/
		static PublicKey* load_string( std::string s ){
			PublicKey* k = new PublicKey();
			k->from_string(s);
			return k;
		};

		/** Create a new PublicKey from a file
			\param fn A path to the file to use
			\returns A pointer to the PublicKey
		*/
		static PublicKey* load_file( std::string fn ){
			PublicKey* k = new PublicKey();
			if(!k->load(fn)){ throw rechain::LoadFailure(fn); }
			return k;
		};

		/** Generate a new PublicKey from a PrivateKey
			\param key The PrivateKey to generate from
		*/
		void generate( std::shared_ptr<PrivateKey> key );

		/** Verify a Data object to ensure that the
		 	signature attached to it is correct
			\param data A shared_ptr to the Data object to verify
			\returns True if the Data object is signed correctly
		*/
		bool verify( std::shared_ptr<Data> data );
};

#endif
