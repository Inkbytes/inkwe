/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <mashad@student.1337.ma>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 09:03:44 by                   #+#    #+#             */
/*   Updated: 2021/09/26 14:25:39 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"
# include "InkSocket.hpp"
# include "InkConfigParser.hpp"
/*
 * This the main function for inkwe webserver
 ** Default config file is located at config.d
*/

int	main(int args, char **argv)
{
	(void)args;
	// Parsing Configuration file
	INKCONFIGPARSER ConfigParser(argv[1]);

	try {
		ConfigParser.readFileData();
		ConfigParser.ParseFileData();
	} catch (std::exception &e){
		std::cout << e.what() << std::endl;
		return (-1);
	}


	return (0);
}