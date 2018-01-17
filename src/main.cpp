#include "interface.hpp"
#include <iostream>

int main( int argc, char** argv ){
	Interface interface(argc,argv);
	return interface.execute();
}
