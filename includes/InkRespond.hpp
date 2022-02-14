/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InkRespond.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-ouar <oel-ouar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/22 09:56:30 by                   #+#    #+#             */
/*   Updated: 2022/02/14 13:54:14 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef INKRESPOND_HPP
# define INKRESPOND_HPP
// Headers
#include "InkCgi.hpp"
#include "InkRequest.hpp"
#include "InkServerConfig.hpp"
#include "InkAutoIndex.hpp"
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>

class InkRespond {
	private:
		std::string	_ret;
		std::string	_status;
		size_t		_err;
		size_t		_cgi;

	public:
		InkRespond(const ft::ServerConfig& conf, const ft::request& req, const std::pair<std::string, int>& a) :_err(0),_cgi(0){
			// check if it's a valid request method
			std::string file;
			if (a.first == "200")
			{
				if (req.getScriptName().length() == 0)
					file = conf.getLocations()[a.second].getRoot() + "/";
				else	
					file = conf.getLocations()[a.second].getRoot() + "/"+ req.getScriptName();
				if (req.getMethod()!= "GET" && req.getMethod()!= "POST" && req.getMethod()!= "DELETE")
				{
					_status="501";
					_ret = "HTTP/1.1 501 Not Implemented\n";
					_err = 1;
				}
	
				// check if it's a valid protocol version
				else if (!req.getServerProtocol().compare("HTTP/1.1"))
				{
					_status = "405";
					_ret = "HTTP/1.1 405 Method Not Allowed\n";
					_err = 1;
				}
				
				// check if path exists
				else if (req.getPath()=="/" && req.getScriptName().length() == 0)
				{
					_status = "index.html";
					_ret = "HTTP/1.1 200 OK\n";
				}
				else if (int d=open(file.c_str(),O_RDONLY)==-1)
				{
					_status = "404";
					_ret ="HTTP/1.1 404 Not Found\n";
					_err = 1;
				}
				
				// check if we will use cgi
				else if ((req.getScriptName().find(".php") != std::string::npos || req.getScriptName().find(".py") != std::string::npos) 
					&& (req.getMethod()=="GET" || req.getMethod()=="POST"))
				{
					std::pair<std::string,std::string> p;
					ft::InkCgi cgi(req, conf.getLocations()[a.second]);
					p = cgi.execCgi(req);
					_status = p.first;
					_ret = p.second;
					if (_status[0] == '4' || _status[0]=='5')
						_err = 1;
					_cgi = 1;
				}
				
				// DELETE request
				else if (req.getMethod()=="DELETE")
				{
					if (remove(req.getScriptName().c_str()) == 0)
					{
						_status = "403";
						_ret = "HTTP/1.1 403 Not Allowed\n";
					}
					_status = "204";
					_ret = "HTTP/1.1 204 No Content\n\n";
				}
				else {
					_ret = "HTTP/1.1 200 OK\n";
					_status = req.getScriptName();
				}
			}
			else 
			{
				_ret = a.first;
				_status = "400";
				_err = 1;
			}
		}

		// Cooking respond
		std::pair<std::string, std::streampos> SetRespond(const ft::request& req, const ft::ServerConfig& conf, std::map<std::string,std::string> types, int locationPos)
		{
			std::string                     opn;
			std::string                     ext;
			std::string                     scriptname;
			std::vector<std::string>        _headers;
			std::streampos                  size;
			std::string                     filePath;
			std::pair<std::string, bool>    Autoindex;
			
			if(locationPos != -1)
			{
				Autoindex = check_autoIndex(req.getPath() + "/" + req.getScriptName() ,conf.getLocations()[locationPos].getRoot()+"/" +req.getScriptName(), conf.getLocations()[locationPos]);;
				if (req.getPath() == "/" && _status == "index.html")
					filePath = conf.getLocations()[locationPos].getRoot() +"/"+_status;
				else
					filePath = conf.getLocations()[locationPos].getRoot()+ "/" +req.getScriptName();
			}
			scriptname = req.getScriptName();
			ext = scriptname.substr(scriptname.find('.')+1,scriptname.length());
			if (Autoindex.second == 1 && Autoindex.first.length() == 0)
				_headers.push_back("Content-Disposition: attachment\r\n");
			if (req.getMethod()=="DELETE" && _status == "204")
				return (std::make_pair(_ret, _ret.length()));
			else if (types.find(ext)!=types.end() && _err==0)
				_headers.push_back("Content-Type: "+types.find(ext)->second+"\r\nContent-Length: ");
			else
				_headers.push_back("Content-Type: text/html; charset=UTF-8\r\nContent-Length: ");
			if (req.getScriptName().find(".php") == std::string::npos || (req.getScriptName().find(".php") != std::string::npos && _err == 1) )
				for (std::vector<std::string>::iterator it = _headers.begin(); it != _headers.end();it++)
					_ret += *it;
			if (_err==1)
				opn = "/Users/mashad/projects/mashad" + conf.getDefaultErrorPagePath()+ "/" +_status+".html";
			else 
				opn = filePath;
			if (_cgi ==1 && _err==0)
			{
				if (req.getScriptName().find(".php") != std::string::npos)
					_ret = _ret +_status;
				else
					_ret += To_string(_status.length()) + "\r\n\r\n" +_status;
				size = _ret.length();
			}
			else
			{
				if (Autoindex.first.length() != 0)
				{
					_ret += To_string(Autoindex.first.length())+"\r\n\r\n" + Autoindex.first;
					size = _ret.length();
				}
				else {
					int tmp;
					std::ifstream f(opn, std::ios::in|std::ios::binary|std::ios::ate);
					size = f.tellg();
					char* file = new char [size];
					f.seekg (0, std::ios::beg);
					f.read(file, size);
					tmp = size;
					_ret += To_string(tmp)+"\r\n\r\n";
					size +=_ret.length();
					for (int i = 0; i < tmp; ++i)
						_ret.push_back(file[i]);
					f.close();
				}
			}
			return (std::make_pair(_ret, size));
		}
		std::pair<std::string, bool>	check_autoIndex(const std::string& urlPath, const std::string& path, const ft::Location& location)
		{
			if (location.getAutoIndex())
			{
				ft::AutoIndex aut(urlPath, path);
				if (opendir(path.c_str()) != nullptr)
				{
					return(std::make_pair(aut.baseHref(), 1));
				}
				else if (int d=open(path.c_str(),O_RDONLY)!=-1)
				{
					return (std::make_pair("", 1));
				}
			}
			
			return (std::make_pair("", 0));
		}
	private:
		template<class T>
    	std::string To_string(T n){
			std::ostringstream convert;
			convert << n;
			return(convert.str());
		}	
};

#endif
