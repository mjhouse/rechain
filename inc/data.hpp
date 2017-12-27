#ifndef DATA_HPP
#define DATA_HPP

#include <tuple>
#include <memory>

enum DataType { Publication, Signature };

typedef std::tuple<std::string,std::string,DataType> Address;

class Data {
	private:
		Address address;
		std::string public_key;
		std::string signature;

	public:
		Data( Address a ) : address(a) {}
		~Data(){}

		void set_address( Address a );
		Address get_address();

		void set_public_key( std::string k );
		std::string get_public_key();

		void set_signature( std::string s );
		std::string get_signature();

		std::string to_string( bool b = false );

		bool verify();
};

#endif
