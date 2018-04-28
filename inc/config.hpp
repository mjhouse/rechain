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

/** \file   config.hpp
	\brief 	Global settings for the application
*/

// system includes
#include <string>
#include <map>

// dependency includes
#include "cereal/types/map.hpp"

#ifndef _RECHAIN_CONFIG_HPP_
#define _RECHAIN_CONFIG_HPP_

/** \brief Maintains global configuration for the application
    and loads/saves the config to a standard location.
*/
class Config {
	private:
        /** The settings as key/value string pairs */
		std::map<std::string,std::string> settings;

        /** A collection of url/port pairs for current peers */
        std::map<std::string,std::string> m_peers;

        /** Flag to stop initialization from happening more than once */
        bool initialized;

	public:

		/** \brief constructor
		*/
		Config();

		/** \brief The destructor
		*/
		~Config();

        /** \brief Set the map of current peers/ports
            \param peers The peers to use
        */
        void set_peers( std::map<std::string,std::string> peers );

        /** \brief Get the map of current peers/ports
            \returns A map of the current peers and ports to connect to
        */
        std::map<std::string,std::string> get_peers();

        /** \brief Create initial settings values
            \returns True if initialization was successful
        */
        bool initialize();

		/** \brief Get a saved value from Config
			\param key The key for the desired value
			\returns A string value
		*/
        std::string setting( std::string key );

		/** \brief Set a value in application Config
			\param key The key for the desired value
			\param value The value to set 
		*/
        void setting( std::string key, std::string value );

		/** Serialize/Unserialize settings
			\param ar The archive to serialize to or from
		*/
		template <class Archive>
		void serialize( Archive& ar ){
			ar(	CEREAL_NVP(settings) );
		}

		/** \brief Save the settings into a file
            \param path The path to save the configuration to
			\returns True if settings were successfully saved
		*/
		bool save( std::string path );

		/** \brief Save the settings into a file
			\returns True if settings were successfully saved
		*/
		bool save();

		/** \brief Load the settings from a file
            \param path The path to load the configuration from
			\returns True if settings were successfully loaded
		*/
		bool load( std::string path);
        
		/** \brief Load the settings from a file
			\returns True if settings were successfully loaded
		*/
		bool load();

};

#endif
