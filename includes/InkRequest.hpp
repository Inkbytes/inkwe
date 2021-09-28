/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/21 11:12:47 by                   #+#    #+#             */
/*   Updated: 2021/09/22 14:44:27 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>

// ALLOWED HTTP METHODS
# define HTTP_GET		"GET"	/* The HTTP GET method requests a representation of the specified resource. Requests using GET should only be used to request data (they shouldn't include data). */
# define HTTP_POST		"POST"	/* The HTTP POST method sends data to the server. The type of the body of the request is indicated by the Content-Type header. */
# define HTTP_HEAD		"HEAD"	/* The HTTP HEAD method requests the headers that would be returned if the HEAD request's URL was instead requested with the HTTP GET method. For example, if a URL might produce a large download, a HEAD request could read its Content-Length header to check the filesize without actually downloading the file. */
# define HTTP_DELETE	"DELETE" /* The HTTP DELETE request method deletes the specified resource. */

class Request
{
	private:
        std::map<std::string, std::string>	Request;

		Request ( void );
	public:
		Request( std::string const &method );
		Request( Request const & );
		~Request( void );
		
		Request 	&operator=( Request const &);
		std::string const &getParsedRequest( void ) const;

};

#endif