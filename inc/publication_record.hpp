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

/**	\file  publication_record.hpp
    \brief Defines the PublicationRecord class
*/

#ifndef _RECHAIN_PUBLICATIONRECORD_HPP_
#define _RECHAIN_PUBLICATIONRECORD_HPP_

// system includes
#include <string>
#include <vector>

// dependency includes
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

#include <boost/serialization/string.hpp>

// local includes
#include "base_record.hpp"

using boost::serialization::base_object;

/** \brief A record that contains publication information for
           a document and is stored in the Blockchain
*/
class PublicationRecord : virtual public BaseRecord {

    private:

        /** The hash of the document being published. 
        */
        std::string m_reference;

        // ------------------------------------------
        // other publication information will go here
        // ------------------------------------------

        /** Make access a friend for serialization */
        friend class boost::serialization::access;

        /** \brief Serialize PublicationRecord to an archive
            \param t_archive The archive to serialize to
            \param int The version of the serialized PublicationRecord
        */
        template <class Archive>
        void serialize( Archive& t_archive, const unsigned int /* version */ ){
            t_archive & base_object<BaseRecord>(*this);
            t_archive & m_reference;
        }

	public:

        /** \brief Empty constructor */
        PublicationRecord();

        /** \brief Construct a publication record from a path */
        PublicationRecord( std::string t_path );

        /** \brief Empty destructor */
        ~PublicationRecord();

        /** \brief Get the reference (hash) of a published document
            \returns The reference to the published document
        */
        std::string get_reference(){ return m_reference; };

        /** \brief Set the reference (hash) of a published document
            \param t_reference The reference to the document
        */
        void set_reference( std::string t_reference ){ m_reference = t_reference; };

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

        /** \brief Get the serialized Record as a string
            \returns The Record as a string
        */
        std::string to_string();


};

BOOST_CLASS_EXPORT_KEY(PublicationRecord)

#endif
