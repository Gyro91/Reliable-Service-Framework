/*
 * server.cpp
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../../include/types.hpp"
#include "../../include/server_class.hpp"

int32_t main (int32_t argc, char_t* argv[])
{	
	uint8_t service = *argv[0], id = *argv[1];
	Server *server;
	std::string server_address("127.0.0.1");

	try {
		server = new Server(id, service, server_address, 5000,
				server_address, 5001);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() <<  std::endl;
		exit(EXIT_FAILURE);
	}	
	server->step();				
	
	std::cout << (int)id << std::endl;
	
	delete server;

	return EXIT_SUCCESS;
}
