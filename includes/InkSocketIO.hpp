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
		typedef std::string string;
		typedef std::vector<Socket> vector;
		typedef struct pollfd pollsd;
		typedef size_t size_type;
		typedef std::map<int, request>	map;
		typedef std::map<int, std::vector<string> > mapi;
		typedef std::map<int, string>				maps;
	private:
		int _timeout;
		int _close_conn;
		int _nfds;
		int _current_size;
		int _desc_ready;
		bool _end_server;
		bool _compress_array;
		vector _sockets;
		map								_reqMap;
		std::map<std::string, std::string> _types;
		struct pollfd _fds[200];
//		map		_req;
		mapi	_reqVec;
		maps	_reqM;

		/** @brief Timestamp method
		 * Return timestamp
		 *
		 * @param none
		 * @return std::time_t
		 */
		std::time_t _getTimestamp(void) const {
			return (std::time(nullptr));
		}


		/** @brief Listen descriptors
		 * Check whether the sd is a listen
		 * descriptor or not
		 *
		 * @param socket descriptor
		 * @return bool, true if it's listen descriptor and false if not
		 */
		bool _isListenSd(int sd) {
			for (int i = 0; i < _sockets.size(); i++) {
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
		Socket *_findSd(int sd) {
			for (int i = 0; i < _nfds; i++) {
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
		Socket *_findCd(int sd) {
			for (int i = 0; i < _sockets.size(); i++) {
				for (int j = 0; j < _sockets.at(i).getClients().size(); j++) {
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
		SocketIO(void) {
			return;
		}

		/** @brief Default constructor
		 * Construct a new Socket io class
		 * to start the servers through polling
		 *
		 * @param vector
		 * @return none
		 */
		SocketIO(vector const &sockets) : _timeout(3 * 60 * 1000), _close_conn(), _nfds(sockets.size()),
										  _current_size(0), _desc_ready(), _end_server(false), _compress_array(false),
										  _sockets(sockets), _reqVec(mapi()), _reqM(maps()), _reqMap(map()){
			// Initializing the fds poll array with zeros.
			std::memset(_fds, 0, sizeof(_fds));
			// Init types
			coock_types();
			// Adding all server sockets into the poll array
			for (int i = 0; i < sockets.size(); i++) {
				_sockets[i].startSocket();
				_fds[i].fd = _sockets[i].getSocketSd();
				_fds[i].events = POLLIN;
			}
			return;
		}

		/** @brief Default destructor
		 * Destroy and clear all private attributes
		 * and memory allocations
		 *
		 * @param none
		 * @return none
		 */
		~SocketIO(void) {
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
		void coock_types(void) {
			std::ifstream file;
			std::stringstream buffer;
			std::string mytext;
			string path;

			char tmp[1024];

			// TO-DO: Get full path
			getcwd(tmp, 1024);

			if (tmp == nullptr)
				throw ft::ServerConfig::GetFolderPathError();

			path = string(tmp);
			file.open(path + "/conf.d/types.txt");
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
		void startServer(void) {
			int new_sd = -1;
			char buffer[1024];
			int len = 1;
			int rc = 1;

			std::cout << "[" << _getTimestamp() << "]: Servers are starting..." << std::endl;
			do {
				// Call poll() and wait 3 minutes for it to complete.
				rc = poll(_fds, _nfds, _timeout);

				// Check to see if the poll call failed.
				if (rc < 0) {
					std::cerr << "[" << _getTimestamp() << "]: Error, Poll failed." << std::endl;
					break;
				}

				if (rc == 0) {
					std::cerr << "[" << _getTimestamp() << "]: Error, timeout." << std::endl;
					break;
				}
				// One or more descriptors are readable. Need to
				// determine which ones they are
				for (int i = 0; i < _nfds; i++) {
					// Loop through to find the descriptors that returned
					// POLLIN and determine whether it's the listening
					// or the active connection.
					if (_fds[i].revents == 0)
						continue;

					if (_isListenSd(_fds[i].fd)) {
						// Listening descriptor is readable.
						std::cout << "[" << _getTimestamp() << "]: " << _fds[i].fd << " Has a incomming connection."
						<< std::endl;
						// Accept all incoming connections that are
						// queued up on the listening socket before we
						// loop back and call poll again
						do {
							// Accept each incoming connection. if
							// accept fails with EWOULDBLOCK, then we
							// have accepted all of them. Any other
							// failure on accept will cause us to end
							// the server.
							new_sd = _findSd(_fds[i].fd)->accepts();
							if (new_sd < 0) {
								break;
							}
							// Add the new incoming connection to
							// the pollfd structure
							std::cout << "[" << _getTimestamp() << "]: New incoming connection " << new_sd
							<< std::endl;
							_fds[_nfds].fd = new_sd;
							_fds[_nfds].events = POLLIN;
							_nfds++;
							// Loop back up and accept another incoming
							// connection.
						} while (new_sd >= 0);
					} else if (_fds[i].revents == POLLIN){
							// Receive data on this connection until the
							// recv fails with EWOULDBLOCK. if any other
							// failure occurs, we will close the connection
							do {
								rc = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
								if (rc < 0)
									break;
								// Check to see if the connection has been
								// closed by the client
								if (rc == 0) {
									std::cout << "[" << _getTimestamp() << "]: Connection closed." << std::endl;
									_close_conn = true;
									break;
								}
								// Data was received
								std::cout << "[" << _getTimestamp() << "]: " << rc << " Bytes received." << std::endl;

								// Request
								ft::Socket *socket = _findCd(_fds[i].fd);
//								socket->getMappedRed()[_fds[i].fd] = "";

								int 						counter;
								string 						myText;
								string 						method;
								const char					*res;

								counter = 0;
								std::stringstream newBuffer(buffer);
								std::memset(&buffer, 0, sizeof(buffer));

								while (getline(newBuffer, myText)) {
									if (_reqM[_fds[i].fd].empty() && counter == 0)
									{
										counter++;
										_reqM[_fds[i].fd] = (myText);
									}
									else
										_reqVec[_fds[i].fd].push_back(myText);
								}
								_reqMap[_fds[i].fd].append(_reqVec[_fds[i].fd], _reqM[_fds[i].fd], socket->getServerConfig());
								_reqMap[_fds[i].fd].parseReq(socket->getServerConfig());

								if (_reqMap[_fds[i].fd].is_complete()) {
									_fds[i].events = POLLOUT;
								}
								_close_conn = true;
							} while (!_close_conn);
					}
					if (_fds[i].revents == POLLOUT) {
							// Getting respond
							ft::Socket *socket = _findCd(_fds[i].fd);
							_reqMap.erase(_fds[i].fd);
							_reqMap[_fds[i].fd].append(_reqVec[_fds[i].fd], _reqM[_fds[i].fd], socket->getServerConfig());
							std::pair<string, int> a = _reqMap[_fds[i].fd].parseReq(socket->getServerConfig());
							InkRespond respond(socket->getServerConfig(), _reqMap[_fds[i].fd], a);
							std::pair<std::string, int> pa = respond.SetRespond(_reqMap[_fds[i].fd], socket->getServerConfig(), _types, a.second);
							rc = send(_fds[i].fd, pa.first.c_str(), pa.second, 0);
							_fds[i].events = POLLIN;
//							_fds[i].revents = POLLIN;
							_reqVec.erase(_fds[i].fd);
							_reqM.erase(_fds[i].fd);

					}else if (_fds[i].revents & (POLLERR | POLLNVAL)) {
						printf("socket error\n");
						break;
					}
				}
			} while (!_end_server);
			// Clean up all of the sockets that are open
			for (int i = 0; i < _nfds; i++) {
				if (_fds[i].fd >= 0)
					close(_fds[i].fd);
			}
		}

	private :
		// anything to string
		template<class T>
		std::string To_string(T n) {
			std::ostringstream convert;
			convert << n;
			return (convert.str());
		}

		void split(std::string text, std::string dilemitter) {
			size_t pos = text.find(dilemitter);
			std::string token = text.substr(0, pos);
			std::string token1 = text.substr(pos + 1, text.length());
			_types.insert(std::pair<std::string, std::string>(token, token1));
		}
	};

}


#endif