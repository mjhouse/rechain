/** \file	keys.hpp
	\brief	Defines the KeyPair class used to manage,
			load, save and generate a public and
			private pair of RSA256 keys.
*/

#ifndef KEYS_HPP
#define KEYS_HPP

#include <cryptopp/rsa.h>
#include <string>

/** Forward declaration for Block */
class BasicBlock;

/** \brief	Stores hex-encoded public and private
			keys as strings. Keys are RSA256.
*/
class KeyPair {
	private:
		CryptoPP::RSA::PublicKey public_key;	/**< the public key */
		CryptoPP::RSA::PrivateKey private_key;	/**< the private key*/

	public:
		/** The constructor */
		KeyPair(){}

		/** Overloaded constructor to load keys from paths
			\param pub_f the path to the public key file
			\param priv_f the path to the private key file
		*/
		KeyPair( std::string pub_f, std::string priv_f );

		/** The destructor */
		~KeyPair(){}

		/** Get the private key
			\returns the hex-encoded private key
		*/
		std::string get_private_key();

		/** Load the private key from a string
			\param key the string to load as the private key
		*/
		void set_private_key( std::string key );

		/** Get the public key
			\returns the hex-encoded public key
		*/
		std::string get_public_key();

		/** Load the public key from a string
			\param key the string to load as the public key
		*/
		void set_public_key( std::string key );

		/** Save keys at the given path.
			\param pub_f the path to the public key file
			\param priv_f the path to the private key file
			\returns true if save is successful.
		*/
		bool save( std::string pub_f, std::string priv_f );

		/** Load keys at the given path.
			\param pub_f the path to the public key file
			\param priv_f the path to the private key file
			\returns true if load is successful
		*/
		bool load( std::string pub_f, std::string priv_f );

		/** Load a private key from the path
			\param fn the path to the private key
			\returns true if key was loaded
		*/
		bool load_private_key( std::string fn );

		/** Load a public key from the path
			\param fn the path to the public key
			\returns true if key was loaded
		*/
		bool load_public_key( std::string fn );

		/** Generate a new key pair.
			\returns true if key generation is successful
		*/
		bool generate();

		/** Check if keys are valid
			\returns True if keys are valid
		*/
		bool is_valid();

		/** Sign a string.
			\param data the string to sign.
			\returns the signature
		*/
		std::string sign( std::string data );

		/** Sign a Block.
			\param block The Block to sign.
			\returns The signature
		*/
		bool sign( std::shared_ptr<BasicBlock> block );

		/** Verify a signature.
			\param data the data that was signed
			\param signature the signature to verify
			\returns true if signature is valid
		*/
		bool verify( std::string data, std::string signature );
};

#endif
