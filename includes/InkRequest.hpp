/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/21 11:12:47 by                   #+#    #+#             */
/*   Updated: 2021/09/27 11:34:06 by                  ###   ########.fr       */
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

#include <iostream>

#include <map>
#include <vector>
#include <iterator>

class request
{
	private:
		std::string _method;
		std::map<std::string, std::string> _details;

	public:
		request(std::vector<std::string> myVec, std::string method) {
			parseRequest(myVec, method);
		}
		~request(void) {}
		void parseRequest(std::vector<std::string> myVec, std::string method)
		{
			_method = method;
			std::map<std::string, std::string>::iterator mite = _details.end();
			std::vector<std::string>::iterator ite = myVec.end();
			// parse every details of the request into a map with akey and value
			for (std::vector<std::string>::iterator it = myVec.begin(); it != ite; it++)
				split(*it, ": ");
			std::cout << "Method  " << _method << std::endl;
			for(std::map<std::string, std::string>::iterator it = _details.begin(); it != mite; it++)
				std::cout << "key " << it->first << "        value " << it->second << std::endl;
		}
	private:
		void split(std::string text, std::string dilemitter)
		{
			size_t pos = text.find(dilemitter);
			std::string token = text.substr(0, pos);
			std::string token1 = text.substr(pos + 2, text.length());
			_details.insert(std::pair<std::string, std::string>(token, token1));
		}
};

#endif