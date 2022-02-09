/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InkSocketIO.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-ouar <oel-ouar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/12 16:05:18 by f0rkr             #+#    #+#             */
/*   Updated: 2022/02/09 10:19:58 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETIO_HPP
# define SOCKETIO_HPP

# include "webserv.hpp"
# include <stdlib.h>

/** @brief ft namespace
 * includes all classes that we need
 */
namespace ft {
	/** @brief socket io class
	 * Contains all methods to execute poll
	 * 
	 * @param none
	 * @return none
	 */
	class SocketIO {
		public:
			typedef std::string			string;
			typedef std::vector<Socket>	vector;
			typedef struct pollfd					pollsd;
			typedef size_t							size_type;

		private:
			int			_timeout;
			int			_close_conn;
			int			_nfds;
			int			_current_size;
			int			_desc_ready;
			bool		_end_server;
			bool		_compress_array;
			vector		_sockets;
			std::map<std::string,std::string>		_types;
			struct pollfd _fds[200];

			/** @brief Timestamp method
			 * Return timestamp
			 *
			 * @param none
			 * @return std::time_t
			 */
			std::time_t		_getTimestamp( void ) const {
				return (std::time(nullptr));
			}


			/** @brief Listen descriptors
			 * Check whether the sd is a listen
			 * descriptor or not
			 *
			 * @param socket descriptor
			 * @return bool, true if it's listen descriptor and false if not
			 */
			bool	_isListenSd( int sd ) {
				for (int i = 0; i < _sockets.size() ; i++)
				{
					if (sd == _sockets[i].getSocketSd())
						return (true);
				}
				return (false);			
			}

			/** @brief Return socket class
			 * Search and return the right socket class
			 * for the incomming socket descriptor
			 *
			 * @param socket descriptors added successfully.
			 * @return Socket
			 */
			Socket		*_findSd( int sd ) {
				for (int i = 0; i < _nfds ; i++)
				{
					if (sd == _sockets[i].getSocketSd())
						return (&(_sockets[i]));
				}
				return (nullptr);
			}

			/** @brief Return socket class
			 * Search and return the right socket class
			 * for the incomming client socket descriptor
			 *
			 * @param socket
			 * @return Socket class
			*/
			Socket		*_findCd( int sd ) {
				for (int i = 0; i < _sockets.size() ; i++) 
				{
					for (int j = 0; j < _sockets.at(i).getClients().size() ; j++) {
						if (sd == _sockets.at(i).getClientsd(j))
							return (&_sockets.at(i));
					}
				}
				return (nullptr);
			}

		public:
			/** @brief Default constructor
			 * Construct an empty Socket io class
			 *
			 * @param none
			 * @return none
			 */
			SocketIO( void ) {
				return ;
			}
			/** @brief Default constructor
			 * Construct a new Socket io class
			 * to start the servers through polling
			 *
			 * @param vector
			 * @return none
			 */
			SocketIO( vector const &sockets ) : _timeout(3 * 60 * 1000), _close_conn(), _nfds(sockets.size()), 
			_current_size(0), _desc_ready(), _end_server(false), _compress_array(false), _sockets(sockets){ 
				// Initializing the fds poll array with zeros.
				std::memset(_fds, 0, sizeof(_fds));
				// Init types
				coock_types();
				// Adding all server sockets into the poll array
				for (int i = 0; i < sockets.size() ; i++) {
					_sockets[i].startSocket();
					_fds[i].fd = _sockets[i].getSocketSd();
					_fds[i].events = POLLIN;
				}
				return ;
			}

			/** @brief Default destructor
			 * Destroy and clear all private attributes
			 * and memory allocations
			 *
			 * @param none
			 * @return none
			 */
			~SocketIO( void ) {
				// Close all server sockets that are still open
				// before ending the server.
                for (int i = 0; i < _sockets.size(); i++) close(_sockets[i].getSocketSd());
				_nfds = 0;
			}
			
			/** @brief coock types
			 * get all content-types
			 *
			 * @param none
			 * @return none
			 */
			void	coock_types( void )
			{
				std::ifstream file;
				std::stringstream buffer;
				std::string mytext;
				
				file.open("Users/mashad/projects/mashad/conf.d/types.txt");
				buffer << file.rdbuf();
				while (getline(buffer, mytext))
					split(mytext, " ");
			}
			
			/** @brief start poll server
			 * Starting poll multiple server
			 *
			 * @param none
			 * @return none
			 */
			void		startServer( void ) {
				int		new_sd = -1;
				char 	buffer[1024];
				int		len = 1;
				int		rc = 1;

				std::cout << "[" << _getTimestamp() << "]: Servers are starting..." << std::endl;
				do
				{
					// Call poll() and wait 3 minutes for it to complete.
					rc = poll(_fds, _nfds, _timeout);

					// Check to see if the poll call failed.
					if (rc < 0)
					{
						std::cerr << "[" << _getTimestamp() << "]: Error, Poll failed." << std::endl;
						break ;
					}

					if (rc == 0)
					{
						std::cerr << "[" << _getTimestamp() << "]: Error, timeout." << std::endl;
						break ;
					}	
					// One or more descriptors are readable. Need to
					// determine which ones they are
                    for (int i = 0; i < _nfds ; i++)
					{
						// Loop through to find the descriptors that returned
						// POLLIN and determine whether it's the listening
						// or the active connection.
						if (_fds[i].revents == 0)
							continue;

						// If revents is not POLLIN, it's an unexpected results,
						// log and end the server.
						if (_fds[i].revents != POLLIN && _fds[i].revents != POLLOUT && _fds[i].revents != (POLLIN | POLLOUT))
						{
							std::cerr << "[" << _getTimestamp() << "]: Error, revents = " << _fds[i].revents << std::endl;
							_end_server = true;
							break ;
						}
						if (_isListenSd(_fds[i].fd))
						{
							// Listening descriptor is readable.
							std::cout << "[" << _getTimestamp() << "]: " << _fds[i].fd << " Has a incomming connection." << std::endl;

							// Accept all incoming connections that are
							// queued up on the listening socket before we
							// loop back and call poll again
							do
							{
								// Accept each incoming connection. if
								// accept fails with EWOULDBLOCK, then we
								// have accepted all of them. Any other
								// failure on accept will cause us to end
								// the server.
								new_sd = _findSd(_fds[i].fd)->accepts();
								if (new_sd < 0)
								{
									if (errno != EWOULDBLOCK)
										_end_server = true;
									break;
								}
								// Add the new incoming connection to
								// the pollfd structure
								std::cout << "[" << _getTimestamp() << "]: New incoming connection " << new_sd << std::endl;
								_fds[_nfds].fd = new_sd;
								_fds[_nfds].events = POLLIN | POLLOUT;
								_nfds++;

								// Loop back up and accept another incoming
								// connection.
							} while (new_sd >= 0);
						}
						// This is not the listening socket, therefore an existing
						// connection must be readable.
						else
						{
							// Receive all incoming data on this socket
							// before we loop back and call poll again.

							do
							{
								// Receive data on this connection until the
								// recv fails with EWOULDBLOCK. if any other
								// failure occurs, we will close the connection
								rc = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
								if (rc < 0)
                                {
                                    if (errno != EWOULDBLOCK)
                                        _end_server = true;
									break;
								}	
								// Check to see if the connection has been
								// closed by the client
								if (rc == 0)
								{
									std::cout << "[" << _getTimestamp() << "]: Connection closed." << std::endl;
									_close_conn = true;
									break;
								}

								// Data was received
								std::cout << "[" << _getTimestamp() << "]: " << rc << " Bytes received." << std::endl;
								
								// Request
								std::pair<std::string, int> a;
								std::string myText;
								std::vector<std::string> myVec;
								std::stringstream NewBuffer((std::string(buffer)));
								std::memset(&buffer, 0, sizeof(buffer));
								std::string method;
								const char *Res;
								int counter = 0;
								std::pair<std::string, int> pa;
								
								// Copy data from buffer to the vector
								while (getline(NewBuffer, myText))
								{
									if (counter == 0)
									{
										counter++;
										method = myText;
									}
									else
										myVec.push_back(myText);
								}
								// getting respond
								request req("localhost");
								a = req.parseRequest(myVec, method, _findCd(_fds[i].fd)->getServerConfig());
								InkRespond respond(_findCd(_fds[i].fd)->getServerConfig(), req, a);
								pa = respond.SetRespond(req, _findCd(_fds[i].fd)->getServerConfig(), _types, a.second);
								rc = send(_fds[i].fd, pa.first.c_str(), pa.second, 0);
								if (rc < 0)
								{
									std::cerr << "[" << _getTimestamp() << "]: Send failed." << std::endl;
									_close_conn = true;
									break ;
								}
								_close_conn = true;
							} while (!_close_conn);

							// If the _close_conn flag was turned on, we need
							// to clean up this active connection. This clean up
							// process includes removing the descriptor.
							if (_close_conn)
							{
								_findCd(_fds[i].fd)->rmClient(_fds[i].fd);
								close(_fds[i].fd);
								_fds[i].fd = -1;
								_compress_array = true;
                                _close_conn = false;
							}
						} /* End of existing connection is readable */
					} /* End of loop through pollable descirptors */

					// If the compress_array flag was turned on. we need
					// to squeeze together the array and decrement the number
					// of file descirptors. We do not need to move back the
					// events and revents fields because the events will always
					// be POLLIN in this case, and revents is output.
					if (_compress_array)
					{
						_compress_array = false;
						for (int i = 0; i < _nfds ; i++)
						{
							if (_fds[i].fd == -1)
							{
								for (int j = i ; j < _nfds ; j++)
									_fds[j].fd = _fds[j + 1].fd;
								i--;
								_nfds--;
							}
						}
					}
				} while (_end_server == false); /* End of server running */

				// Clean up all of the sockets that are open
				for (int i = 0; i < _nfds; i++)
				{
					if (_fds[i].fd >= 0)
						close(_fds[i].fd);
				}
			}
		private :
			// anything to string
			template<class T>
    		std::string To_string(T n){
				std::ostringstream convert;
				convert << n;
				return(convert.str());
			}	
			void split(std::string text, std::string dilemitter)
			{
				size_t pos = text.find(dilemitter);
				std::string token = text.substr(0, pos);
				std::string token1 = text.substr(pos + 1, text.length());
				_types.insert(std::pair<std::string, std::string>(token, token1));
			}  
	};

}

#endif
