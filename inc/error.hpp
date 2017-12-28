#ifndef _ERROR_HPP_
#define _ERROR_HPP_

#include <exception>
#include <sstream>

namespace rechain {

	class LoadFailure : public std::runtime_error {
		private:
			std::string path;

		public:
			LoadFailure( std::string p ) : std::runtime_error("failed to load path"), path(p) {}

			virtual const char* what() const throw() {
				std::stringstream stream;
				stream << runtime_error::what() << ": " << get_path();

				return stream.str().c_str();
			}

			std::string get_path() const { return path; }

	};
}

#endif
