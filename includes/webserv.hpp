/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <mashad@student.1337.ma>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 08:32:16 by                   #+#    #+#             */
/*   Updated: 2021/09/26 12:24:14 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

// Including necessary libraries

# include <sys/select.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/event.h>
# include <arpa/inet.h>
# include <iostream>
# include <unistd.h>
# include <fstream>
# include <sstream>
# include <fcntl.h>
# include <cstdlib>
# include <poll.h>
# include <cstdio>
# include <ctime>

// End of including

//----- // Define Marcos for general use

# define DEFAULT_CONFIG_PATH 	"config/default.conf.conf"
# define DEFAULT_PORT			80
# define DEFAULT_ROOT_DIRECTIVE "www"
# define DEFAULT_404_PAGE       "pages/404.html"
# define DEFAULT_501_PAGE       "pages/501.html"
# define DEFAULT_500_PAGE       "pages/500.html"

//----- // End of Macros

#endif
