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

/**	\file  signature_record.hpp
    \brief Defines the SignatureRecord class
*/

#ifndef _RECHAIN_SIGNATURERECORD_HPP_
#define _RECHAIN_SIGNATURERECORD_HPP_

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
class SignatureRecord : virtual public BaseRecord {

    private:

        /** The hash of the record being signed. 
        */
        std::string m_record_hash;

        // ------------------------------------------
        // other signature information will go here
        // ------------------------------------------

        /** Make access a friend for serialization */
        friend class boost::serialization::access;

        /** \brief Serialize SignatureRecord to an archive
            \param t_archive The archive to serialize to
            \param int The version of the serialized SignatureRecord
        */
        template <class Archive>
        void serialize( Archive& t_archive, const unsigned int /* version */ ){
            t_archive & base_object<BaseRecord>(*this);
            t_archive & m_record_hash;
        }

	public:

        /** \brief Empty constructor */
        SignatureRecord();

        /** \brief Construct a signature from a hash */
        SignatureRecord( std::string t_hash );

        /** \brief Empty destructor */
        ~SignatureRecord();

        /** \brief Get the reference-hash of a PublicationRecord
            \returns The reference to the PublicationRecord
        */
        std::string get_record_hash(){ return m_record_hash; };

        /** \brief Set the hash of the referenced PublicationRecord
            \param t_record_hash The hash of the PublicationRecord
        */
        void set_record_hash( std::string t_record_hash ){ m_record_hash = t_record_hash; };

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

BOOST_CLASS_EXPORT_KEY(SignatureRecord)

#endif
