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

/**	\file  genesis_record.hpp
    \brief Defines the GenesisRecord class
*/

#ifndef _RECHAIN_GENESISRECORD_HPP_
#define _RECHAIN_GENESISRECORD_HPP_

// system includes
#include <string>
#include <vector>

// dependency includes
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

// local includes
#include "base_record.hpp"

using boost::serialization::base_object;

/** \brief A class that acts as the initial Record in a Blockchain, storing
           information about the Blockchain itself, and acting as an initial
           distributor of trust during trust calculations.
*/
class GenesisRecord : virtual public BaseRecord {

    private:

        /** The name of the Blockchain in which this GenesisRecord
            is held.
        */
        std::string m_name;

        /** \brief A list of users to distribute starting
                   trust to during the trust calculation
        */
        std::vector<std::string> m_distribution;

        /** Make access a friend for serialization */
        friend class boost::serialization::access;

        /** \brief Serialize GenesisRecord to an archive
            \param t_archive The archive to serialize to
            \param int The version of the serialized GenesisRecord
        */
        template <class Archive>
        void serialize( Archive& t_archive, const unsigned int /* version */ ){
            t_archive & base_object<BaseRecord>(*this);
            t_archive & m_name;
            t_archive & m_distribution;
        }

	public:

        /** \brief Empty constructor */
        GenesisRecord();

        /** \brief Empty destructor */
        ~GenesisRecord();

        /** \brief Get the name for the Blockchain
            \returns The name of the Blockchain
        */
        std::string get_name(){ return m_name; };

        /** \brief Set the name for the Blockchain
            \param t_name The name to set for the new Blockchain
        */
        void set_name( std::string t_name ){ m_name = t_name; };

        /** \brief Get the distribution list for the GenesisRecord
            \returns The distribution list for the GenesisRecord
        */
        std::vector<std::string> get_distribution(){ return m_distribution; }

        /** \brief Set the distribution list for the GenesisRecord
            \param t_distribution The distribution list for the GenesisRecord
        */
        void set_distribution( std::vector<std::string> t_distribution ){ m_distribution = t_distribution; }

        /** \brief Get the RecordType of this Record
            \returns The RecordType of this Record
        */
        int get_type();

        /** \brief Check if Record is internally valid
            \returns True if Record is valid
        */
        bool is_valid();

        /** \brief Get the concatenated data for signing
            \returns The data of the record as a string
        */
        std::string get_data();

        /** \brief Get the serialized record as a string
            \returns The record as a string
        */
        std::string to_string();


};

BOOST_CLASS_EXPORT_KEY(GenesisRecord)

#endif
