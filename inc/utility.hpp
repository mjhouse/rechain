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

/** \file   utility.hpp
	\brief 	Global helper functions 
*/

// system includes
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <stdio.h>
#include <locale>

// dependency includes
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

#ifndef _RECHAIN_UTILITY_HPP_
#define _RECHAIN_UTILITY_HPP_

namespace rechain {

    /** \brief Trim whitespace from the left side of a string
        \param s A reference to the string to trim
    */
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                std::not1(std::ptr_fun<int, int>(std::isspace))));
    }

    /** \brief Trim whitespace from the right side of a string
        \param s A reference to the string to trim
    */
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }

    /** \brief Trim whitespace from both sides of a string
        \param s A reference to the string to trim
    */
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    /** \brief Trim whitespace from the left side of a string
        \param s The string to trim
        \returns A trimmed copy of the string
    */
    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    /** \brief Trim whitespace from the right side of a string
        \param s The string to trim
        \returns A trimmed copy of the string
    */
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    /** \brief Copy a file from one location to another
        \param from The full path of the source file
        \param to The full path to the new location
        \param overwrite True if copy should overwrite
        \returns True if copy was successful
    */
    static inline bool copy_file( std::string from, std::string to, bool overwrite ){
        if(overwrite)
            remove(to.c_str());

        std::ifstream ifs(from,std::ios::binary);
        std::ofstream ofs(to,std::ios::binary);
        if(ifs.is_open() && ofs.is_open()){
            ofs << ifs.rdbuf();
            return true;
        }
        return false;
    }

    /** \brief Copy a file from one location to another
        \param from The boost::filesystem::path of the source file
        \param to The full boost::filesystem::path to the new location
        \param overwrite True if copy should overwrite
        \returns True if copy was successful
    */
    static inline bool copy_file( fs::path from, fs::path to, bool overwrite ){
        return copy_file(from.string(),to.string(),overwrite);
    }
}

#endif
