#pragma once

// system includes
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>

/** Enum for different log levels */
enum Level { info, debug, warning, error };

static const std::string STDOUT = "__stdout__"; /**< A key to identify STDOUT v. file path */
static const std::string STDERR = "__stderr__";	/**< A key to identify STDERR v. file path */

class Log{
	private:
		std::string name;	/**< The name to identify this log by */
		std::string out;	/**< The path to the log file */
		Level level;		/**< The log level to write at */

		/** Get a timestamp as a string
			\returns A timestamp as a string
		*/
		inline std::string timestamp(){
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
		inline std::string format( std::string m ){
			std::string formatted;

			formatted.append("[" + timestamp() + "]: ");
			formatted.append(m);

			return formatted;
		}

	public:

		/** Constructor
			\param n The name to identify this log by
			\param o The path to write to
			\param l The log level to write at
		*/
		Log( std::string n, std::string o, Level l ) : name(n), out(o), level(l) {}

		/** Empty destructor */
		~Log(){}

		/** Compare the name of this log to a string
			\param n The string to compare to
			\returns True if the given string matches name
		*/
		bool named( std::string n ){
			return (!name.empty() && n == name);
		}

		/** Write to this log
			\param m The message to write
			\param l The Level to write at
		*/
		void write( std::string m, Level l ){
			if(l <= level){
				if(out == STDOUT)
					std::cout << format(m) << std::endl;
				else if(out == STDERR)
					std::cerr << format(m) << std::endl;
				else {
					std::ofstream ofs(out,std::ios::app);
					if(ofs.is_open()){
						ofs << format(m) << std::endl;
					}
				}
			}
		}

};

/** The Logger is a singleton class used to write to 
	a collection of logs.
*/
class Logger {
	private:
		std::vector<Log> logs;			/**< The collection of Log objects */
		std::vector<Log>::iterator current;	/**< An iterator to the current Log */

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
			[&s]( Log& l ){
				return (l.named(s));
			});
			return l;
		}

		/** Add a Log to the Logger
			\param l The Log to add
			\returns A reference to the Logger
		*/
		Logger& with( const Log& l ){
			logs.push_back(l);
			current = logs.end();
			return *this;
		}

		/** Write an info-level message to the logs
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& info( std::string m ){
			write("Info: " + m,Level::info);
			return *this;
		}

		/** Write a debug-level message to the logs
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& debug( std::string m ){
			write("Debug: " + m,Level::debug);
			return *this;
		}
		
		/** Write a warning-level message to the logs
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& warning( std::string m ){
			write("Warning: " + m,Level::warning);
			return *this;
		}
	
		/** Write an error-level message to the logs
			\param m The message to write
			\returns A reference to the Logger
		*/
		Logger& error( std::string m ){
			write("Error: " + m,Level::error);
			return *this;
		}
};