#ifndef DATA_HPP
#define DATA_HPP

#include <tuple>
#include <memory>

enum DataType { Publication, Signature };

typedef std::tuple<std::string,std::string,DataType> Address;

class Data: public std::enable_shared_from_this<Data> {
	private:
		Address address;
		std::string public_key;
		std::string signature;

	public:
		Data(){}
		Data( Address a ) : address(a) {}
		~Data(){}

		void set_address( Address a );
		Address get_address();

		void set_public_key( std::string k );
		std::string get_public_key();

		void set_signature( std::string s );
		std::string get_signature();

		void set_data_ref( std::string r );
		std::string get_data_ref();

		void set_block_ref( std::string r );
		std::string get_block_ref();

		void set_data_type( DataType d );
		DataType get_data_type();

		std::string to_string( bool b = false );

		bool verify();
};

#endif
