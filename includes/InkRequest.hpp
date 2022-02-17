/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InkRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-ouar <oel-ouar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/21 11:12:47 by                   #+#    #+#             */
/*   Updated: 2022/02/17 07:46:04 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <map>
# include <vector>
# include <iterator>
# include <exception>
# include "webserv.hpp"
# include "InkServerConfig.hpp"

namespace ft {
	class request
	{
		private:
			std::string 						_method;
			std::string 						_serverProtocol;
			std::string 						_path;
			std::string 						_query;
			std::string 						_clientIp;
			std::string 						_scriptName;
			std::map<std::string, std::string> 	_details;
			bool								_complete;
			std::vector<std::string>			_request;
			ft::ServerConfig						_conf;

		public:
			request( void ) : _clientIp("localhost"), _complete(false), _request(std::vector<std::string>()), _method("") {
				return ;
			}
			request(std::string clientIp, const std::string &method, ft::ServerConfig conf) : _clientIp(clientIp), _complete(false), _method(method) { }
			~request(void) {}
			std::pair<std::string, int> parseRequest(std::vector<std::string> myVec, std::string method, ft::ServerConfig conf)
			{
				std::string body;
				method.erase(method.find('\r'));

				splitMethod(method);
				int pos = splitPath(_path, conf);
				std::vector<std::string>::iterator ite = myVec.end();
				std::vector<std::string>::iterator it = myVec.begin();
				if (conf.getLocations()[pos].getMethod() != _method)
					return (std::make_pair("HTTP/1.1 400 BAD REQUEST\n", -1));
				// parse every details of the request into a map with akey and value
				for (; it != ite; it++)
				{
					if ((*it).find(": ") == std::string::npos)
					{
						it++;
						break ;
					}
					else
					{
						(*it).erase((*it).find('\r'));
						if (split(*it, ": ") == -1)
							return (std::make_pair("HTTP/1.1 400 BAD REQUEST\n", -1));

					}
				}
				// set POST body
				if (_query.length()==0 && it != ite && _method == "POST")
				{
					// chunked request
					if (_details.find("Transfer-Encoding")->second == "chunked")
					{
						std::vector<int> chunck_size;
						(*it).erase((*it).find('\r'));
						if (is_chunck_length(*it) == -1)
							return (std::make_pair("HTTP/1.1 400 BAD REQUEST\n", -1));
						else
						{
							chunck_size.push_back(is_chunck_length(*it));
							it++;
						}
						for (;it!=ite;it++)
						{
							(*it).erase((*it).find_last_of('\r'));
							if (is_chunck_length(*it) != -1)
							{
								if (is_chunck_length(*it) == 0)
								{
									_complete = 1;
									break ;
								}
								if (_query.length() > total_size(chunck_size))
									return (std::make_pair("HTTP/1.1 400 BAD REQUEST\n", -1));
								else if (_query.length() < total_size(chunck_size))
									_query += *it;
								else
									chunck_size.push_back(is_chunck_length(*it));
							}
							else
								_query += *it;
						}
					}
					else
					{
						for (;it!=ite;it++)
							_query+= *it;
					}
				} else
					_complete = 1;
				return (std::make_pair("200", pos));
			}

			// Append request and check if is complete
			void 	append(std::vector<std::string> const &req, const std::string &method, ft::ServerConfig conf) {
				for (int i = 0 ; i < req.size() ; i++) {
					_request.push_back(req[i]);
				}
				_method = method;
			}

			std::pair<std::string, int>	parseReq(ft::ServerConfig conf) {
				_conf = conf;
				return (parseRequest(_request, _method, conf));
			}
			// is_chunk_length imp
			int is_chunck_length(std::string str) {
				str.substr(0, str.length()-1);
				if (is_hex(str) != 0)
					return (hex_to_dec(str));
				try {stoi(str);}
				catch (std::exception & e) {
					return(-1);
				}
				return stoi(str);
			}

			// getters for class private attributes
			std::string getMethod( void ) const {
				return (_method);
			}
			std::string getPath( void ) const {
				return (_path);
			}
			std::string getServerProtocol( void ) const {
				return (_serverProtocol);
			}
			std::string getQuery( void ) const {
				return (_query);
			}
			std::string	getClientIp( void ) const {
				return (_clientIp);
			}
			std::string getScriptName( void ) const {
				return (_scriptName);
			}
			std::map<std::string, std::string> getDetails( void ) const {
				return (_details);
			}
			bool getComplet( void ) const {
				return (_complete);
			}
			// check if request is complete
			bool is_complete(){
				if (_complete)
					return (1);
				return (0);
			}
			bool is_hex(std::string hex){
				for (int i =0; i < hex.length();i++)
					if (isxdigit(hex[i]) == 0)
						return (0);
				return (1);
			}
			int hex_to_dec(std::string hex)
			{
				int x;
				std::stringstream stream;

				stream << hex;
				stream >> std::hex >> x;
				return (x);
			}
			int total_size(std::vector<int> chunck_size)
			{
				int x = 0;

				for (std::vector<int>::iterator it = chunck_size.begin(); it != chunck_size.end();it++)
					x+= *it;
				return (x);
			}
			//private methods
		private:
			int split(std::string text, std::string dilemitter)
			{
				size_t pos = text.find(dilemitter);
				std::string token = text.substr(0, pos);
				std::string token1 = text.substr(pos + 2, text.length());
				if (token.empty() || token1.empty())
					return (-1);
				_details.insert(std::pair<std::string, std::string>(token, token1));
				return (1);
			}
			void splitMethod(std::string method)
			{
				size_t pos = method.find(" ");
				_method = method.substr(0, pos);
				method.erase(0, pos + 1);
				pos = method.find(" ");
				_path = method.substr(0, pos);
				method.erase(0, pos);
				_serverProtocol = method.substr(0, method.length());
			}
			int splitPath(std::string path, ft::ServerConfig conf)
			{
				int pos = 0;
				std::string p = path;
				int ret = 0;

				// set GET querry
				if ((pos = path.find("?")) > 0)
				{
					_query = path.substr(pos + 1, path.length());
					path.erase(pos, path.length());
					p=path;
				}

				while (path.find("/") != std::string::npos)
				{
					_path = path;
					for (int i = 0; i <conf.getLocationsCount(); i++)
					{
						if (conf.getLocations()[i].getLocation() == "/")
							ret = i;
						if (conf.getLocations()[i].getLocation() == _path && conf.getLocations()[i].getMethod() == _method)
						{
							_scriptName = p.substr(_path.length(), p.length());
							if (_scriptName == "/")
								_scriptName = "";
							else if (_scriptName.find("/") != std::string::npos)
								_scriptName = _scriptName.substr(_scriptName.find("/")+1,_scriptName.length());
							return (i);
						}
					}
					if (path.find_first_of("/") == path.find_last_of("/"))
						path = "/";
					else
						path = path.substr(0,path.find_last_of("/"));
				}
				_path = "/";
				_scriptName = p.substr(p.find("/")+1, p.length());
				return (ret);
			}
	};
}
