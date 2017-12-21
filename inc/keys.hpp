#ifndef KEYS_HPP
#define KEYS_HPP

#include <string>

class KeyPair {
	private:
		std::string public_key;
		std::string private_key;

	public:
		KeyPair(){}
		KeyPair( std::string fn );
		~KeyPair(){}

		std::string get_private_key(){ return this->private_key; }
		std::string get_public_key(){ return this->public_key; }

		bool save( std::string fn );

		bool load( std::string fn );

		bool generate();
};

#endif
