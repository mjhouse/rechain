#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main( int argc, char* argv[] ){
	// There must be exactly one instance of Session
	Catch::Session session;

	// writing to session.configData() here sets defaults
	// this is the preferred way to set them
	int code = session.applyCommandLine( argc, argv );
	if( code != 0 )
		return code;

	// writing to session.configData() or session.Config() here
	// overrides command line args
	int failed = session.run();
	return failed;
}
