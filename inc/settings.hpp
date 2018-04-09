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

/** \file	settings.hpp
	\brief 	Global settings for the application
*/

// system includes
#include <string>
#include <map>

// dependency includes
#include "cereal/types/map.hpp"

#ifndef _RECHAIN_SETTINGS_HPP_
#define _RECHAIN_SETTINGS_HPP_

/** \brief Maintains global configuration for the application
    and loads/saves the config to a standard location.
*/
class Settings {
	private:
        /** The settings as key/value string pairs */
		std::map<std::string,std::string> settings = {
            { "home", "" },
            { "config", "" },
            { "log", "" }
        };		

        bool initialized;


	public:

		/** \brief Private constructor
		*/
		Settings();

		/** \brief The destructor
		*/
		~Settings(){}

		/** \brief Get a saved value from Settings
			\param key The key for the desired value
			\returns A value of type 'T'
		*/
		template <typename T>
		T get( std::string key );

		/** \brief Get a saved value from Settings
			\param key The key for the desired value
			\returns A string value
		*/
        std::string gets( std::string key );

		/** \brief Set a value in application Settings
			\param key The key for the desired value
			\param value The value to set 
		*/
		template <typename T>
		void set( std::string key, T value );

		/** \brief Set a value in application Settings
			\param key The key for the desired value
			\param value The value to set 
		*/
        void sets( std::string key, std::string value );

		/** Serialize/Unserialize settings
			\param ar The archive to serialize to or from
		*/
		template <class Archive>
		void serialize( Archive& ar ){
			ar(	CEREAL_NVP(this->settings) );
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
