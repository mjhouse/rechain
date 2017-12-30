#ifndef _ERROR_HPP_
#define _ERROR_HPP_

#include <exception>
#include <sstream>

/** ReChain namespace to prevent collisions
	with exceptions from libraries
*/
namespace rechain {

	/** An exception that is thrown when an object
		is unable to load from a file.
	*/
	class LoadFailure : public std::runtime_error {
		private:
			/** The path that failed to load */
			std::string path;

		public:
			/** Constructor that takes a path to the failed file
				\param p The path that failed to load
			*/
			LoadFailure( std::string p ) : std::runtime_error("failed to load path"), path(p) {}

			/** An overloaded 'what' method
				\returns A failure message for the exception
			*/
			virtual const char* what() const throw() {
				std::stringstream stream;
				stream << runtime_error::what() << ": " << get_path();

				return stream.str().c_str();
			}

			/** A getter for the path variable
				\returns The path as a string
			*/
			std::string get_path() const { return path; }

	};

}

#endif
