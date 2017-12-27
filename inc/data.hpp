#ifndef DATA_HPP
#define DATA_HPP

#include <sstream>

enum DataType {
	Publication,
	Signature
};

class Data {
	private:
		std::string reference;
		std::string public_key;
		DataType datatype;

		std::string signature;

	public:
		Data(){}

		Data( std::string ref, DataType d ){
			this->reference = ref;
			this->datatype	= d;
		}

		~Data(){}

		void set_reference( std::string r ){ this->reference = r; }
		std::string get_reference(){ return this->reference; }

		void set_public_key( std::string k ){ this->public_key = k; }
		std::string get_public_key(){ return this->public_key; }

		void set_datatype( DataType d ){ this->datatype = d; }
		DataType get_datatype(){ return this->datatype; }

		void set_signature( std::string s ){ this->signature = s; }
		std::string get_signature(){ return this->signature; }

		std::string to_string( bool b = false ){
			std::stringstream stream;
			stream	<< this->reference
					<< this->public_key
					<< std::to_string(this->datatype);

			if(b) stream << this->signature;
			return stream.str();
		}
};

#endif
