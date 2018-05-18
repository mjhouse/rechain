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

/**	\file  base_record.hpp
    \brief Defines a abstract BaseRecord class
*/

#ifndef _RECHAIN_BASERECORD_HPP_
#define _RECHAIN_BASERECORD_HPP_

// system includes
#include <string>

// dependency includes
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>

/** \brief The BaseRecord class acts as an abstract base class
           for other kinds of records.
*/
class BaseRecord {

    private:

        /** Make access a friend for serialization */
        friend class boost::serialization::access;

        /** \brief Serialize BaseRecord to an archive
            \param t_archive The archive to serialize to
        */
        template <class Archive>
        void serialize( Archive& t_archive, const unsigned int /* version */ ){
            t_archive & m_public_key;
            t_archive & m_signature;
            t_archive & m_nonce;
            t_archive & m_timestamp;
            t_archive & m_counter;
        }

    protected:

        // data variables
        std::string m_public_key;         /**< The public key of the owner */
        std::string m_signature;		  /**< The signature */
        std::string m_previous;	          /**< The signature */

        // hashing variables
        long m_nonce;		           	  /**< Randomly generated value to modify hash */
        long m_timestamp;			      /**< A timestamp */
        uint32_t m_counter;               /**< Counter to modify hash output */


	public:

        /** \brief Empty constructor */
        BaseRecord();

        /** \brief Empty destructor */
        virtual ~BaseRecord(){};

        /** Get the hash of this BaseRecord
            \returns The current hash of the BaseRecord
        */
        std::string hash();

        /** \brief Re-hash until the hash is valid
            \returns A valid hash
        */
        std::string mine();

        /** \brief Check if BaseRecord is internally valid
            \returns True if BaseRecord is valid
        */
        virtual bool is_valid();

        /** \brief Get the concatenated data for hashing/signing
            \returns The data of the record as a string
        */
        virtual std::string get_data();

        /** \brief Get the RecordType of this Record
            \returns The RecordType of this Record
        */
        virtual int get_type() = 0;

        /** \brief Get the serialized Record as a string
            \returns The Record as a string
        */
        virtual std::string to_string() = 0;

        /** \brief Get the hash of the previous record 
            \returns The hash of the previous record
        */
        std::string get_previous(){ return m_previous; }

        /** \brief Set the hash of the previous record
            \param t_previous The hash to set
        */
        void set_previous( std::string t_previous ){ m_previous = t_previous; }

        /** \brief Get the public key
            \returns The value of m_public_key
        */
        std::string get_public_key(){ return m_public_key; }

        /** \brief Set the public key
            \param t_key The public key to use
        */
        void set_public_key( std::string t_key ){ m_public_key = t_key; }

        /** \brief Get the signature
            \returns The value of m_signature
        */
        std::string get_signature(){ return m_signature; }

        /** \brief Set the signature
            \param t_signature The signature to use
        */
        void set_signature( std::string t_signature ){ m_signature = t_signature; }

        /** \brief Get the random number used in hashing
            \returns The nonce
        */
        long get_nonce(){ return m_nonce; }

        /** \brief Get the timestamp of the BaseRecord
            \returns The raw posix timestamp
        */
        long get_timestamp(){ return m_timestamp; }

        /** \brief Get the counter used in hashing
            \returns The counter
        */
        uint32_t get_counter(){ return m_counter; }

};


#endif
