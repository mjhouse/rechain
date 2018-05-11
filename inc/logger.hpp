// system includes
#include <iostream>
#include <algorithm>
#include <set>
#include <string>
#include <fstream>
#include <ctime>

// dependency includes
#include <boost/lexical_cast.hpp>

// local includes
#include "enums.hpp"

#ifndef _RECHAIN_LOGGER_HPP_
#define _RECHAIN_LOGGER_HPP_

static const std::string STDOUT = "__stdout__"; /**< A key to identify STDOUT v. file path */
static const std::string STDERR = "__stderr__";	/**< A key to identify STDERR v. file path */

#define RCINFO( s )    ( Logger::get().info( __FILE__, __LINE__, s ) )
#define RCDEBUG( s )   ( Logger::get().debug( __FILE__, __LINE__, s ) )
#define RCWARNING( s ) ( Logger::get().warning( __FILE__, __LINE__, s ) )
#define RCERROR( s )   ( Logger::get().error( __FILE__, __LINE__, s ) )

/** A log that writes to a file or stderr/stdout
*/
class Log {
	private:
		std::string m_name;	/**< The name to identify this log by */
		std::string m_out;	/**< The path to the log file */
		Level m_level;		/**< The log level to write at */

		/** Get a timestamp as a string
			\returns A timestamp as a string
		*/
		inline std::string timestamp() const {
			time_t now = time(0);
			std::string dt = std::string(ctime(&now));

			dt.erase(std::remove_if(dt.begin(),dt.end(),[]( unsigned char c ){
				return (c == '\n' || c == '\t');
			}), dt.end());

			return dt;
		}

		/** Format a string before writing to the log
			\param m The string to format
			\returns The now-formatted string
		*/
		inline std::string format( std::string m ) const {
			std::string formatted;

			formatted.append("[" + timestamp() + "] ");
			formatted.append(m);

			return formatted;
		}

	public:

		/** Constructor
			\param n The name to identify this log by
			\param o The path to write to
			\param l The log level to write at
		*/
		Log( std::string n, std::string o, Level l ) : m_name(n), m_out(o), m_level(l) {}

		/** Empty destructor */
		~Log(){}

		/** Get the log name
			\returns The log name
		*/
		std::string name() const {
			return m_name;
		}

        /** Get the log level
            \returns A Level enum value
        */
        Level level() const {
            return m_level;
        }

		/** Write to this log
			\param m The message to write
			\param l The Level to write at
		*/
		void write( std::string m, Level l ) const {
            if(l >= m_level){
				if(m_out == STDOUT)
					std::cout << format(m) << std::endl;
				else if(m_out == STDERR)
					std::cerr << format(m) << std::endl;
				else {
					std::ofstream ofs(m_out,std::ios::app);
					if(ofs.is_open()){
						ofs << format(m) << std::endl;
					}
				}
			}
		}

        /** Overloaded not-equal comparison
            \param rhs The log to compare with
            \returns True if logs are not equal
        */
        bool operator!=( const Log& rhs ) const {
            return !(*this == rhs);
        }

        /** Overloaded equal comparison
            \param rhs The log to compare with
            \returns True if logs are equal
        */
        bool operator==( const Log& rhs ) const {
            return (m_name == rhs.m_name && m_out == rhs.m_out && m_level == rhs.m_level);
        }

        /** Overloaded less-than comparison
            \param rhs The log to compare with
            \returns True if this < rhs
        */
        bool operator<( const Log& rhs ) const {
            return (m_name < rhs.m_name);
        }

        /** Overloaded greater-than comparison
            \param rhs The log to compare with
            \returns True if this > rhs
        */
        bool operator>( const Log& rhs ) const {
            return (m_name > rhs.m_name);
        }

        /** Overloaded copy/assignment operator
            \param rhs The log to copy
        */
        void operator=( const Log& rhs ){
            m_name  = rhs.m_name;
            m_out   = rhs.m_out;
            m_level = rhs.m_level;
        }
};

/** The Logger is a singleton class used to write to
	a collection of logs.
*/
class Logger {
	private:
		std::set<Log> logs;			        /**< The collection of Log objects */
		std::set<Log>::iterator current;    /**< An iterator to the current Log */

		/** Empty constructor */
		Logger(){}

		/** Write a message to the logs
			\param m The message to write
			\param l The Level to write at
		*/
		void write( std::string m, Level l ){
			if(current != logs.end()){
				(*current).write(m,l);
			} else {
				for(auto s : logs) s.write(m,l);
			}
		}

	public:

		/** Empty destructor */
		~Logger(){}

		/** Get the Logger with or without a current log set
			\param s A log to set as current
			\returns A reference to the Logger
		*/
		static Logger& get( std::string s = "" ){
			static Logger l;
			l.current = std::find_if(l.logs.begin(),l.logs.end(),
			[&s]( const Log& l ){
				return (l.name() == s);
			});
			return l;
		}

		/** Add a Log to the Logger
			\param l The Log to add
			\returns A reference to the Logger
		*/
		Logger& with( const Log& l ){
            logs.insert(l);
			current = logs.end();
			return *this;
		}

		/** Write an info-level message to the logs
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& info( std::string m ){
            write("INFO: " + m,Level::info);
			return *this;
		}

		/** Write an info-level message to the logs
			\param f The file of the caller
			\param l The line number of the caller
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& info( const char* f, const int l, std::string m ){
			std::string file(f,std::strlen(f));
			std::string line = boost::lexical_cast<std::string>(l);
			return info(file + ": " + line + ": " + m);
		}

		/** Write a debug-level message to the logs
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& debug( std::string m ){
			write("DEBUG: " + m,Level::debug);
			return *this;
		}

		/** Write an debug-level message to the logs
			\param f The file of the caller
			\param l The line number of the caller
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& debug( const char* f, const int l, std::string m ){
			std::string file(f,std::strlen(f));
			std::string line = boost::lexical_cast<std::string>(l);
			return debug(file + ": " + line + ": " + m);
		}

		/** Write a warning-level message to the logs
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& warning( std::string m ){
			write("WARNING: " + m,Level::warning);
			return *this;
		}

		/** Write an warning-level message to the logs
			\param f The file of the caller
			\param l The line number of the caller
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& warning( const char* f, const int l, std::string m ){
			std::string file(f,std::strlen(f));
			std::string line = boost::lexical_cast<std::string>(l);
			return warning(file + ": " + line + ": " + m);
		}

		/** Write an error-level message to the logs
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& error( std::string m ){
			write("ERROR: " + m,Level::error);
			return *this;
		}

		/** Write an error-level message to the logs
			\param f The file of the caller
			\param l The line number of the caller
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& error( const char* f, const int l, std::string m ){
			std::string file(f,std::strlen(f));
			std::string line = boost::lexical_cast<std::string>(l);
			return error(file + ": " + line + ": " + m);
		}
};

#endif
