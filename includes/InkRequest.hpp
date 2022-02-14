/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InkRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-ouar <oel-ouar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/21 11:12:47 by                   #+#    #+#             */
/*   Updated: 2022/02/14 14:03:10 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <map>
# include <vector>
# include <iterator>
# include "InkServerConfig.hpp"

namespace ft {
	class request {
	private:
		std::string _method;
		std::string _serverProtocol;
		std::string _path;
		std::string _query;
		std::string _clientIp;
		std::string _scriptName;
		bool _isvalid; // True If the request is valid and complete, false otherwise
		std::map<std::string, std::string> _details;

	public:
		request(std::string clientIp) : _clientIp(clientIp), _isvalid(false) {}

		~request(void) {}

		std::pair<std::string, int>
		parseRequest(std::vector<std::string> myVec, std::string method, ft::ServerConfig conf) {
			method.erase(method.find('\r'));
			splitMethod(method);
			int pos = splitPath(_path, conf);
			std::vector<std::string>::iterator ite = myVec.end();
			std::vector<std::string>::iterator it = myVec.begin();
			if (conf.getLocations()[pos].getMethod() != _method)
				return (std::make_pair("HTTP/1.1 400 BAD REQUEST\n", -1));
			// parse every details of the request into a map with akey and value
			for (; it != ite; it++) {
				if ((*it).find(": ") == std::string::npos) {
					it++;
					break;
				} else {
					(*it).erase((*it).find('\r'));
					split(*it, ": ");
				}
			}
			// set POST body
			if (_query.length() == 0 && it != ite && _method == "POST")
				for (; it != ite; it++)
					_query += *it;
			return (std::make_pair(SUCCESS_OK, pos));
		}

		// getters for class attributes
		std::string getMethod(void) const {
			return (_method);
		}

		std::string getPath(void) const {
			return (_path);
		}

		std::string getServerProtocol(void) const {
			return (_serverProtocol);
		}

		std::string getQuery(void) const {
			return (_query);
		}

		std::string getClientIp(void) const {
			return (_clientIp);
		}

		std::string getScriptName(void) const {
			return (_scriptName);
		}

		std::map<std::string, std::string> getDetails(void) const {
			return (_details);
		}

		//private methods
	private:
		void split(std::string text, std::string dilemitter) {
			size_t pos = text.find(dilemitter);
			std::string token = text.substr(0, pos);
			std::string token1 = text.substr(pos + 2, text.length());
			_details.insert(std::pair<std::string, std::string>(token, token1));
		}

		void splitMethod(std::string method) {
			size_t pos = method.find(" ");
			_method = method.substr(0, pos);
			method.erase(0, pos + 1);
			pos = method.find(" ");
			_path = method.substr(0, pos);
			method.erase(0, pos);
			_serverProtocol = method.substr(0, method.length());
		}

		int splitPath(std::string path, ft::ServerConfig conf) {
			int pos = 0;
			std::string p = path;
			int ret = 0;

			// set GET querry
			if ((pos = path.find("?")) > 0) {
				_query = path.substr(pos + 1, path.length());
				path.erase(pos, path.length());
				p = path;
			}

			while (path.find("/") != std::string::npos) {
				_path = path;
				for (int i = 0; i < conf.getLocationsCount(); i++) {
					if (conf.getLocations()[i].getLocation() == "/")
						ret = i;
					if (conf.getLocations()[i].getLocation() == _path &&
						conf.getLocations()[i].getMethod() == _method) {
						_scriptName = p.substr(_path.length(), p.length());
						if (_scriptName == "/")
							_scriptName = "";
						else if (_scriptName.find("/") != std::string::npos)
							_scriptName = _scriptName.substr(_scriptName.find("/") + 1, _scriptName.length());
						return (i);
					}
				}
				if (path.find_first_of("/") == path.find_last_of("/"))
					path = "/";
				else
					path = path.substr(0, path.find_last_of("/"));
			}
			_path = "/";
			_scriptName = p.substr(p.find("/") + 1, p.length());
			return (ret);
		}
	};
}
