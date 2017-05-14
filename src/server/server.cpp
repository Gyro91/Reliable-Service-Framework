/*
 * server.cpp
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../../include/types.hpp"
#include "../../include/server_class.hpp"
#include <zmq.hpp>
#include <unistd.h>


int32_t main(int32_t argc, char_t* argv[])
{	
	uint8_t service = *argv[0], id = *argv[1];
	Server *server;
	std::string server_address("*");
	std::string broker_address("localhost");

	try {
		server = new Server(id, service, server_address, 5555,
				broker_address, 6000);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() <<  std::endl;
		exit(EXIT_FAILURE);
	}
	
	server->step();				
	
	delete server;

	return EXIT_SUCCESS;
}
