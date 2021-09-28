/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ink_configParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <mashad@student.1337.ma>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/26 08:26:13 by                   #+#    #+#             */
/*   Updated: 2021/09/27 11:14:40 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef INKCONFIGPARSER_HPP
#define INKCONFIGPARSER_HPP

# include "InkServerConfig.hpp"
# include <webserv.hpp>

class INKCONFIGPARSER
{
	private:
		std::string				_filename; // Configuration file name
		size_t					_serversCount; // Servers counter
		std::string				_rawData; // Raw file data
		INKSERVERCONFIG			*_inkweConfig; // Configuration key and values holder

		// Private methods declaration.
		size_t 				_GetServersCount( void ) { // Return server counts
			std::stringstream	data(this->_rawData);
			std::string			line;
			size_t 				serversCount;

			serversCount = 0;
			while (std::getline(data, line, '\n')) {
				if (line[0] == '[')
					serversCount++;
			}
			return (serversCount);
		}
		void 				_GetLocationCount( void ) {
			std::stringstream	data(this->_rawData);
			std::string			line;
			size_t 				locationsCount;
			size_t 				serversCount;

			locationsCount = 0;
			serversCount = 0;
			while (std::getline(data, line, '\n')) {
				if (line[0] == '#' || line[0] == '\0')
					continue ;
				while (std::getline(data, line, '\n') && line[0] != '[')
				{
					if (line.find("location=") != std::string::npos)
						locationsCount++;
				}
				this->_inkweConfig[serversCount].setLocationsCount(locationsCount);
				this->_inkweConfig[serversCount++].setInkLocations(locationsCount);
				locationsCount = 0;
			}
			return ;
		}
	public:
		INKCONFIGPARSER( void ): _filename("conf.d/default.conf") {
			this->_serversCount = 0;
			this->_inkweConfig = new INKSERVERCONFIG();
			return ;
		}
		INKCONFIGPARSER ( std::string const &filename ): _filename(filename) {
			this->_serversCount = 0;
			return ;
		}
		INKCONFIGPARSER( INKCONFIGPARSER const &COPY) {
			*this = COPY;
		}
		~INKCONFIGPARSER ( void ) {
			return ;
		}

		INKCONFIGPARSER		&operator=(INKCONFIGPARSER const &OP) {
			this->_filename = OP.getFileName();
			return (*this);
		}
		size_t				getServerCount( void ) const {
			return (this->_serversCount);
		}
		std::string			getFileName( void ) const {
			return (this->_filename);
		}
		void 				readFileData( void ) {
			std::ifstream		file;
			std::stringstream	buffer;

			// TO-DO: Read Configuration file and store it into a data string
			file.open(this->_filename); // Opening Configuration file
			if (!file.good()) {
				throw INKCONFIGPARSER::FileOpeningError(); // Throwing FileOpeningError Exception in case there's an error in opening the configuration file
				return ;
			}
			buffer << file.rdbuf(); // Reading configuration file data
			this->_rawData = buffer.str(); // Putting raw string file data into _rawData attribute
		}

		void 				ParseFileData() {
			std::stringstream	data(this->_rawData);
			std::string			line;
			size_t				ServersCounter;
			size_t 				LocationsCounter;

			// TO-DO: Parse configuration data from _rawData string and store into inkweConfig
			ServersCounter = -1;
			LocationsCounter = 0;
			this->_serversCount = this->_GetServersCount(); // Get server count in configuration file
			this->_inkweConfig = new INKSERVERCONFIG[this->_serversCount]; // Allocate InkweConfig as size
			this->_GetLocationCount();
			while (std::getline(data, line)) {
				if (line[0] == '#' || line[0] == '\0')
					continue;
				if (line[0] == '[') {
					LocationsCounter = 0;
					this->_inkweConfig[++ServersCounter].setServerName(std::strtok(&line[1], "]"));
				}
				else
				{
					if (line.find(';') == std::string::npos) {
						std::cout << line[line.length() - 2] << " =  " << line.back() << std::endl;
						throw INKCONFIGPARSER::FileParsingError();
					}
					// TO-DO : switch line cases and fill the inkweConfig class with the right attributes
					switch (line[0]) {
						case 'h': {
							if (line.find("host=") == std::string::npos){
								throw INKCONFIGPARSER::FileParsingError();
							}
							this->_inkweConfig[ServersCounter].setHost(std::strtok(&line[5], ";"));
							break ;
						}
						case 'p' : {
							if (line.find("port=") == std::string::npos){
								throw INKCONFIGPARSER::FileParsingError();
							}
							this->_inkweConfig[ServersCounter].setPort(std::atoi(std::strtok(&line[5], ";")));
							break ;
						}
						case 'd' : {
							if (line.find("default_error_pages=") == std::string::npos){
								throw INKCONFIGPARSER::FileParsingError();
							}
							this->_inkweConfig[ServersCounter].setDefaultErrorPage(std::strtok(&line[5], ";"));
							break ;
						}
						case 'b': {
							if (line.find("bodysize_limit=") == std::string::npos){
								throw INKCONFIGPARSER::FileParsingError();
							}
							this->_inkweConfig[ServersCounter].setBodySizeLimit(std::atoi(std::strtok(&line[5], ";")));
							break ;
						}
						case 'l': {
							if (line.find("location=") == std::string::npos){
								throw INKCONFIGPARSER::FileParsingError();
							}
							this->_inkweConfig[ServersCounter].getInkLocation()[LocationsCounter++].ParseInkLocation(line.substr(8, line.length()));
							break ;
						}
					}
				}
			}
		}
		class FileOpeningError : public std::exception{
			public:
				FileOpeningError( void ) throw(){
					return ;
				}
				virtual ~FileOpeningError( void ) throw(){
					return ;
				}
				virtual const char* what() const throw(){
					return ("Error: Can't Open Configuration file.");
				}
		};
		class FileParsingError : public std::exception{
			public:
				FileParsingError( void ) throw(){
					return ;
				}
				virtual ~FileParsingError( void ) throw(){
					return ;
				}
				virtual const char * what() const throw(){
					return ("Error: Can't parse line.");
				}
		};

};

#endif
