/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InkRespond.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-ouar <oel-ouar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/22 09:56:30 by                   #+#    #+#             */
/*   Updated: 2022/02/25 18:51:10 by oel-ouar         ###   ########.fr       */
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

namespace ft {
	class InkRespond {
		public:
			typedef size_t size_type;
			typedef std::string string;
		private:
			string 			_ret;
			string 			_status;
			size_type		_err;
			size_type		_cgi;
			size_type		_file_size;
			size_type		_current_size;
			std::ifstream 	_stream;
			size_type		_header_size;
			bool 			_flag;
			InkRespond& 	operator=(const InkRespond& op);
		public:
			InkRespond(void) : _err(0), _cgi(0), _file_size(0), _current_size(0), _flag(false){
				return;
			}
			InkRespond(const InkRespond &copy): _err(0), _cgi(0), _file_size(0), _current_size(0), _flag(false) {
				return ;
			}
			explicit InkRespond(const ft::ServerConfig &conf, const ft::request &req, const std::pair<std::string, int> &a) : _err(
					0), _cgi(0), _file_size(0), _current_size(0), _header_size(0), _flag(false) {
				return ;
			}

			// Set respond
			void confRespond(const ft::ServerConfig &conf, const ft::request &req, const std::pair<std::string, int> &a) {
				_err = 0;
				_cgi = 0;
				_file_size = 0;
				_current_size = 0;
				_flag = false;
				// check if its a valid request nethod
				std::string file;
				if (a.first == "200")
				{
					if (req.getScriptName().length() == 0)
						file = conf.getLocations()[a.second].getRoot() + "/";
					else	
						file = conf.getLocations()[a.second].getRoot() + "/"+ req.getScriptName();
					
					// check if path exists
					if (req.getPath()=="/" && req.getScriptName().length() == 0)
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
						&& (req.getMethod()=="GET" || req.getMethod()=="POST") && (conf.getLocations()[a.second]).getAutoIndex() == 0)
					{
						std::pair<std::string,std::string> p;
						InkCgi cgi(req, conf.getLocations()[a.second]);
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
					_status = To_string(a.second);
					_err = 1;
				}
			}

			// Cooking respond
			std::pair<std::string, std::streampos>
			SetRespond(const ft::request &req, const ft::ServerConfig &conf, std::map<std::string, std::string> types,
					   int locationPos) {
				std::string opn;
				std::string ext;
				std::string scriptname;
				std::vector<std::string> _headers;
				std::streampos size;
				std::string filePath;
				std::pair<std::string, bool> Autoindex;
				
				if(!_err)
				{
					Autoindex = check_autoIndex(req.getPath() + "/" + req.getScriptName() ,conf.getLocations()[locationPos].getRoot()+"/" +req.getScriptName(), conf.getLocations()[locationPos]);;
					if (req.getPath() == "/" && _status == "index.html")
						filePath = conf.getLocations()[locationPos].getRoot() +"/"+_status;
					else
						filePath = conf.getLocations()[locationPos].getRoot()+ "/" +req.getScriptName();
				}
				scriptname = req.getScriptName();
				if (scriptname.find('.') != std::string::npos)
					ext = scriptname.substr(scriptname.find('.') + 1, scriptname.length());
				else
					ext = "";
				if (Autoindex.second == 1 && Autoindex.first.length() == 0)	
					_headers.push_back("Content-Disposition: attachment\r\n");
				if (req.getMethod()=="DELETE" && _status == "204")
					return (std::make_pair(_ret, _ret.length()));
				else if (types.find(ext)!=types.end() && _err==0)
					_headers.push_back("Content-Type: "+types.find(ext)->second+"\r\nContent-Length: ");
				else
					_headers.push_back("Content-Type: text/html; charset=UTF-8\r\nContent-Length: ");
				if ((req.getScriptName().find(".php") != std::string::npos && (_err == 1 || Autoindex.second == 1)) || req.getScriptName().find(".php") == std::string::npos) {
					for (std::vector<std::string>::iterator it = _headers.begin(); it != _headers.end();it++)
						_ret += *it;
					_header_size = _ret.length();
				}
				if (_err==1 && conf.getDefaultErrorPagePath() != "")
					opn = conf.getFullPath() + conf.getDefaultErrorPagePath()+ "/" +_status+".html";
				else if (_err == 1)
				{
					_ret = _ret + "3\r\n\r\n"+ _status;
					return (std::make_pair(_ret, _ret.length()));
				}
				else 
					opn = filePath;
				/// here

				if (_cgi ==1 && _err==0 && Autoindex.second == 0)
				{
					if (req.getScriptName().find(".php") != std::string::npos)
						_ret = _ret +_status;
					else
						_ret += To_string(_status.length()) + "\r\n\r\n" +_status;
					size = _ret.length();
					_flag = true;
				} else {
					if (Autoindex.first.length() != 0)
					{
						_ret += To_string(Autoindex.first.length())+"\r\n\r\n" + Autoindex.first;
						size = _ret.length();
						_flag = true;
					} else {
						// MOD
						int i;
						int tmp;
						_stream.open(opn, std::ios::in | std::ios::binary | std::ios::ate);

						std::cout << opn << std::endl;
						_file_size = _stream.tellg();
						_current_size = 0;
						size = 0;
						_stream.seekg(0, std::ios::beg);
						_ret += To_string(_file_size) + "\r\n\r\n";
						size += _ret.length();
						// END MOD
					}
				}
				return (std::make_pair(_ret, size));
			}
			std::pair<std::string, std::streampos>	readStream() {
				char 			file[2048*100];
				int 			tmp;
				int				size;
				int 			i;
				std::string	ret = "";

				std::cout << "READING FILE"<< std::endl;
				std::memset(&file, 0, 2048*100);
				for (i = 0; i < 2048*100 && !_stream.eof() ; i++)
					_stream.get(file[i]);
				tmp = i;
				size = tmp;
				
				for (i = 0; i < tmp; i++)
					ret.push_back(file[i]);
				std::memset(file, 0, 2048*10);
				std::cout << "READING IS DONE" << std::endl;
				return (std::make_pair(ret, size));
			}

			void 		streamClose( void ) {
				_stream.close();
				_ret.clear();
				return ;
			}
			size_type	getHeaderSize( void ) const {
				return (_header_size);
			}
			std::pair<std::string, bool>
			check_autoIndex(const std::string &urlPath, const std::string &path, const ft::Location &location) {
				int d = -1;
				
				if (location.getAutoIndex()) {
					ft::AutoIndex aut(urlPath, path);
					if (opendir(path.c_str()) != nullptr) {
						return (std::make_pair(aut.baseHref(), 1));
					} else if ((d = open(path.c_str(), O_RDONLY)) != -1) {
						return (std::make_pair("", 1));
					}
				}
				close(d);
				return (std::make_pair("", 0));
			}

			bool is_done(size_type position) {
				if (_stream.eof() || _flag == true) {
					std::cout << _flag << std::endl;
					std::cout << " HERE " << std::endl;
					_flag = false;
					return (true);
				}
				_current_size += position;
				_stream.seekg(_current_size, std::ios::beg);
				return (false);
			}

		private:
			template<class T>
			std::string To_string(T n) {
				std::ostringstream convert;
				convert << n;
				return (convert.str());
			}
	};
}
#endif
