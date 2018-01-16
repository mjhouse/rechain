#include "interface.hpp"
#include <iostream>

int main( int argc, char** argv ){
	Interface interface(argc,argv);
	interface.execute();

	return 0;
}
