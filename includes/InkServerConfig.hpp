/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InkServerConfig.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <mashad@student.1337.ma>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/26 10:03:55 by                   #+#    #+#             */
/*   Updated: 2021/09/28 08:42:21 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef INKSERVERCONFIG_HPP
# define INKSERVERCONFIG_HPP

# include <iostream>
# include <string>

class INKLOCATION {
	private:
		std::string		_location; // Location path
		std::string		_method; // Allowed method to use in this location
		std::string		_root;	// Root file paths
		bool 			_autoIndex; // Enable or Disable Directory listing
	public:
		INKLOCATION( void ){
			this->_location = "";
			this->_method = "";
			this->_root = "";
			this->_autoIndex = false;
			return ;
		}
		~INKLOCATION( void )
		{
			return ;
		}
		void 	setLocation(std::string const &location) {
			this->_location = location;
			return ;
		}
		void 	setMethod(std::string const &method) {
			this->_method = method;
			return ;
		}
		void 	setRoot(std::string const &root) {
			this->_root = root;
			return ;
		}
		void 	setAutoIndex(bool autoindex) {
			this->_autoIndex = autoindex;
			return ;
		}
		bool 		ParseInkLocation(std::string rawData) {
			std::stringstream	data(rawData);
			std::string			line;

			std::cout << rawData << std::endl;
			if (rawData[0] != '{' )
				return (false);
			while (std::getline(data, line, ',')){
				std::cout << line << std::endl;
			}
			return (true);
		}
};
class INKSERVERCONFIG {
	private:
		std::string 	_serverName; // Server name
		std::string		_host; // IP address to bind on
		size_t			_port; // Port number to listen in
		size_t			_bodySizeLimit; // Request body size limit
		size_t 			_locationsCount; // Location counter
		std::string		_defaultErrorPage; // Path to default error pages
		INKLOCATION		*_inkLocations; // Location class


	public:
		INKSERVERCONFIG( void ){
			this->_host = "";
			this->_port = 0;
			this->_bodySizeLimit = 0;
			this->_defaultErrorPage = "pages/";
			this->_locationsCount = 0;
			return ;
		}
		INKSERVERCONFIG( INKSERVERCONFIG const &COPY ) : _port(), _bodySizeLimit(), _inkLocations() {
			*this = COPY;
			return ;
		}
		~INKSERVERCONFIG( void ) {
			return ;
		}
		std::string		getServerName( void ) const{ // Server Name getter
			return (this->_serverName);
		}
		std::string 	getHost( void ) const { // IP address getter
			return (this->_host);
		}
		std::string 	getDefaultErrorPage( void ) const { // Get default Error page
			return (this->_defaultErrorPage);
		}
		size_t 			getPort( void ) const { // Get port number
			return (this->_port);
		}
		size_t 			getBodySizeLimit( void ) const {  // Get request body size limit
			return (this->_bodySizeLimit);
		}
		size_t 			getLocationsCount( void ) const { // Get Locations Counter
			return (this->_locationsCount);
		}
		INKLOCATION		*getInkLocation( void ) const { // Get Array of location object
			return (this->_inkLocations);
		}
		void 			setServerName( std::string const &serverName ) {
			this->_serverName = serverName;
			return ;
		}
		void 			setHost( std::string const &host ) {
			this->_host = host;
			return ;
		}
		void 			setPort( size_t port ) {
			this->_port = port;
			return ;
		}
		void 			setDefaultErrorPage( std::string const &defaultErrorPage ) {
			this->_defaultErrorPage = defaultErrorPage;
			return ;
		}
		void 			setBodySizeLimit( size_t bodySizeLimit ) {
			this->_bodySizeLimit = bodySizeLimit;
			return ;
		}
		void 			setLocationsCount( size_t locationsCount ) {
			this->_locationsCount = locationsCount;
		}
		void 			setInkLocations(size_t locationCount){
			this->_inkLocations = new INKLOCATION[locationCount];
			return ;
		}
};
#endif
