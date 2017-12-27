#include "keys.hpp"
#include "data.hpp"

#include <iostream>

void display( std::shared_ptr<Data>& data ){
	std::cout 	<< "Data Object: " << std::endl
				<< "Reference:   " << data->get_reference()  << std::endl
				<< "Public Key:  " << data->get_public_key() << std::endl
				<< "Data Type:   " << data->get_datatype()	 << std::endl
				<< "Signature:	 " << data->get_signature()  << std::endl;
}

int main( int argc, char** argv ){

	std::shared_ptr<Data> data(new Data("KJDFDGKDFIIYDFTRKEHTIE7YFVDNDGKHU", DataType::Publication));

	std::shared_ptr<PublicKey> public_key(new PublicKey("data/rsa.public"));
	std::shared_ptr<PrivateKey> private_key(new PrivateKey("data/rsa.private"));

	data->set_public_key( public_key->to_string() );
	private_key->sign(data);

	return 0;
}
