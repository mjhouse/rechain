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

// dependency includes
#include <boost/filesystem/path.hpp>

// local includes
#include "blockchain.hpp"
#include "config.hpp"

class PrivateKey;

/** \brief Executes operations against the blockchain
    and holds the private and public keys.
*/
class Manager {
	private:
        
        bool m_configured;                              /**< This flag is true if Manager is configured*/

		std::shared_ptr<PrivateKey> m_private_key;      /**< A pointer to the current private key */
		Blockchain m_blockchain;                        /**< A Blockchain to load data into */

        /** \brief Setup the home directory
            \returns True if setup was successful
        */
        bool setup();

    public:

		/** \brief Constructor
		*/
		Manager();

		/** \brief Destructor
		*/
		~Manager();

		/** \brief Configure the program environment
            \param level The level to log at
            \returns True if home directory is configured
		*/
		bool configure(Level level);

		/** \brief Publish a given document
			\param t_path The path to the file
			\returns True on success
		*/
		bool publish( std::string t_path );

		/** \brief Sign a previously published document
			\param t_hash The hash of the Record to sign
			\returns True on success
		*/
		bool sign( std::string t_hash );

		/** \brief Add or generate a private key
			\param t_path The path to the key file
			\returns True on success
		*/
		bool key( std::string t_path );

        /** \brief Save the current blockchain
            \returns True on success
        */
        bool save();

        /** \brief Save the blockchain to a new location
            \returns True on success
        */
        bool save( std::string t_path );

        /** \brief Load the blockchain
            \returns True on success
        */
        bool load();

        /** \brief Load the blockchain from a new location
            \returns True on success
        */
        bool load( std::string t_path );

		/** \brief Check the Blockchain is valid
		    \returns true if blockchain is valid
        */
		bool is_valid();

};

#endif
