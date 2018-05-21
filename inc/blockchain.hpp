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

/**	\file  	blockchain.hpp
	\brief 	Defines the Blockchain class that 
		manages BaseRecord objects, searches
		and verifies the BaseRecord collection. 
*/

#ifndef _RECHAIN_BLOCKCHAIN_HPP_
#define _RECHAIN_BLOCKCHAIN_HPP_

// system includes
#include <vector>
#include <memory>
#include <map>

// dependency includes
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// local includes
#include "base_record.hpp"
#include "genesis_record.hpp"
#include "publication_record.hpp"
#include "signature_record.hpp"

/** The Blockchain class manages a collection of 
    BaseRecord objects.
*/
class Blockchain {
	private:

        /** The collection of records in the Blockchain */
		std::vector< std::shared_ptr<BaseRecord> > m_blockchain; 

        /** The collection of records in the Blockchain */
		std::map<std::string,double> m_trust;

        /** Maximum trust in the Blockchain */
        double max_trust;

        /** Minimum trust in the Blockchain */
        double min_trust;

		/** Update the trust_map */	
		void update_trust();	

        /** Make access a friend for serialization */
        friend class boost::serialization::access;

        /** \brief Serialize Blockchain to an archive
            \param t_archive The archive to serialize to
            \param int The version of the serialized Blockchain
        */
        template <class Archive>
        void serialize( Archive& t_archive, const unsigned int /* version */ ){
            t_archive & m_blockchain;
        }

	public:

		/** Typedef a Blockchain iterator */
		typedef std::vector< std::shared_ptr<BaseRecord> >::iterator iterator;

		/** Empty constructor
		*/
		Blockchain();

		/** Empty destructor
		*/
		~Blockchain();

        /** \brief Mine, add and broadcast a record
            \param t_record The record to mine, add and broadcast
            \returns True if the record was published
        */
        bool publish( std::shared_ptr<BaseRecord> t_record );

		/** \brief Find a BaseRecord by the hash
			\param t_hash The hash of the BaseRecord to return
			\returns A pointer to a BaseRecord object
		*/
		std::shared_ptr<BaseRecord> find_record( std::string t_hash );

		/** \brief Find a BaseRecord by the reference
			\param t_reference The reference of the PublicationRecord to return
			\returns A pointer to a PublicationRecord object
		*/
		std::shared_ptr<PublicationRecord> find_publication( std::string t_reference );

		/** \brief Find signatures by the publication reference
			\param t_reference The reference of the signatures
			\returns A vector of SignatureRecord objects
		*/
		std::vector< std::shared_ptr<SignatureRecord> > find_signatures( std::string t_reference );

		/** Verify that the Blockchain is valid
			\returns True if Blockchain is valid
		*/
		bool is_valid();

		/** Get the trust for a published BaseRecord or user
			\param t_identifier The BaseRecord hash or user public key
			\returns The trust for the user or BaseRecord
		*/
		double trust( std::string t_identifier );
		
		/** Return an iterator to the start of the Blockchain
			\returns An iterator
		*/
		Blockchain::iterator begin();

		/** Returns an iterator to the end of the Blockchain
		\returns A vector iterator
		*/ 
		Blockchain::iterator end();

		/** Return the number of BaseRecord objects in the chain
			\returns The number of BaseRecord objects 
		*/
		size_t size();	

		/** Save the Blockchain to a given location
			\param t_path The path to save to
			\returns True if the Blockchain was saved
		*/
		bool save( std::string t_path );
		
		/** Load the Blockchain from a given location
			\param t_path The path to load from
			\returns True if the Blockchain was loaded and is valid
		*/
		bool load( std::string t_path );

};

#endif
