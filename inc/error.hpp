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

#ifndef _ERROR_HPP_
#define _ERROR_HPP_

#include <exception>
#include <sstream>

/** ReChain namespace to prevent collisions
	with exceptions from libraries
*/
namespace rechain {

	/** An exception that is thrown when an object
		is unable to load from a file.
	*/
	class LoadFailure : public std::runtime_error {
		private:
			/** The path that failed to load */
			std::string path;

		public:
			/** Constructor that takes a path to the failed file
				\param p The path that failed to load
			*/
			LoadFailure( std::string p ) : std::runtime_error("failed to load path"), path(p) {}

			/** An overloaded 'what' method
				\returns A failure message for the exception
			*/
			virtual const char* what() const throw() {
				std::stringstream stream;
				stream << runtime_error::what() << ": " << get_path();

				return stream.str().c_str();
			}

			/** A getter for the path variable
				\returns The path as a string
			*/
			std::string get_path() const { return path; }

	};

}

#endif
