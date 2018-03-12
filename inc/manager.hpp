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

/** \file	manager.hpp
	\brief 	Handles requests from a cli or gui
*/

#ifndef _RECHAIN_MANAGER_HPP_
#define _RECHAIN_MANAGER_HPP_

// system includes
#include <string>
#include <iostream>

// local includes
#include "blockchain.hpp"
#include "keys.hpp"

/** Executes operations against the blockchain
    and holds the private and public keys.
*/
class Manager {
	private:
		std::string home;					            /**< The path to the home directory */

		std::shared_ptr<PrivateKey> private_key;		/**< A pointer to the current private key */
		std::shared_ptr<PublicKey> public_key;			/**< A pointer to the current public key */

		BlockChain blockchain;				        	/**< A BlockChain to load data into */

		/** Publish a given record 
			\param r The Record to publish
			\returns True on success
		*/
		bool publish( Record& r );

		/** Add a private key 
			\param k Add a key object
			\returns True on success
		*/
		void set_private_key( PrivateKey* k );

		/** Add a public key 
			\param k Add a key object
			\returns True on success
		*/
		void set_public_key( PublicKey* k );

    public:

		/** Constructor
            \param home The path to the install directory
		*/
		Manager( std::string home );

		/** Destructor
		*/
		~Manager();

		/** Configure the program environment 
            \returns True if home directory is configured
		*/
		bool configure();

		/** Publish a given document
			\param s The path to the file
			\returns True on success
		*/
		bool publish( std::string s );
		
		/** Find a document by reference
			\param h The hash of the file
			\returns True on success
		*/
		Record request( std::string h );

		/** Add or generate a private key 
			\param p The path to the key file
			\returns True on success
		*/
		void set_private_key( std::string p );

        
		/** Add or generate a public key 
			\param p The path to the key file
			\returns True on success
		*/
		void set_public_key( std::string p );
       
        
		/** Sign a previously published document
			\param s The reference hash to sign
			\returns True on success
		*/
		bool sign( std::string s );
	
		/** Check the BlockChain is valid
		    \returns true if blockchain is valid
        */
		bool validate();

};

#endif
