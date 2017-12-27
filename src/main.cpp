#include "keys.hpp"
#include "data.hpp"

#include <iostream>

void display( std::shared_ptr<Data>& data ){
	Address addr = data->get_address();
	std::cout 	<< "Data Object:     "	<< std::endl
				<< "	Public Key:  " 	<< data->get_public_key()	<< std::endl
				<< "	Signature:	 " 	<< data->get_signature()	<< std::endl
				<< "	Verified:	 " 	<< data->verify()			<< std::endl
				<< "Address:         "	<< std::endl
				<< "	Reference:   " 	<< std::get<0>(addr)		<< std::endl
				<< "	Block Hash:  " 	<< std::get<1>(addr)		<< std::endl
				<< "	Data Type:   " 	<< std::get<2>(addr)		<< std::endl;
}

int main( int argc, char** argv ){
	std::shared_ptr<Data> data( new Data( Address( "REFERENCE", "BLOCK HASH", DataType::Signature ) ) );

	std::shared_ptr<PublicKey> public_key( new PublicKey("data/rsa.public") );
	std::shared_ptr<PrivateKey> private_key( new PrivateKey("data/rsa.private") );

	private_key->sign(data);
	display(data1);

	return 0;
}
