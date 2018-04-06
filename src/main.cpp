#include "interface.hpp"
#include <iostream>

int main( int argc, char** argv ){
	Interface interface(argc,argv);
	int result = interface.execute();
    return result;
}
