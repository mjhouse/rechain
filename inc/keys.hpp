/** \file	keys.hpp
	\brief	Defines the Key classes used to manage,
			load, save and generate a public and
			private pair of RSA256 keys.
*/

#ifndef KEYS_HPP
#define KEYS_HPP

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
			try {
				this->key.Load(CryptoPP::StringSource(k, true, new CryptoPP::HexDecoder()).Ref());
			} catch (CryptoPP::BERDecodeErr& e){ _OUT(e.what()); }
		}

		/** Load a key from a file
			\param fn The path to the file
		*/
		void load( std::string fn ){
			std::ifstream file(fn);
			if( file.is_open() ){
				this->from_string( std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()) );
			} else { _OUT("Can't load '" + fn + "'"); }
		}

		/** Save the key to a file
			\param fn The file to save to
		*/
		void save( std::string fn ){
			std::ofstream file(fn);
			if( file.is_open() ){
				file << this->to_string();
			} else { _OUT("Can't save '" + fn + "'"); }
		}
};

/** The PrivateKey class inherits from the templated
	'Key' base class and adds private-key-specific methods.
*/
class PrivateKey : public Key<CryptoPP::RSA::PrivateKey>, public std::enable_shared_from_this<PrivateKey> {
	public:
		/** Empty constructor */
		PrivateKey(){}

		/** Constructor to load key from file on
			creation
			\param fn The file to load
		*/
		PrivateKey( std::string fn );

		/** Empty destructor */
		~PrivateKey(){}

		/** Generate a new key */
		void generate();

		/** Sign a given Data block
			\param data A shared_ptr to the Data object to sign
		*/
		void sign( std::shared_ptr<Data> data );
};

/** The PublicKey class inherits from the templated
	'Key' base class and adds public-key-specific methods.
*/
class PublicKey: public Key<CryptoPP::RSA::PublicKey>, public std::enable_shared_from_this<PublicKey> {
	public:
		/** Empty constructor */
		PublicKey(){}

		/** Constructor to load key from file on
			creation
			\param fn The file to load
		*/
		PublicKey( std::string fn );

		/** Empty destructor */
		~PublicKey(){}

		/** Generate a new PublicKey from a PrivateKey
			\param key The PrivateKey to generate from
		*/
		void generate( std::shared_ptr<PrivateKey> key );

		/** Verify a Data object to ensure that the
		 	signature attached to it is correct
			\param data A shared_ptr to the Data object to verify
		*/
		bool verify( std::shared_ptr<Data> data );
};

#endif
