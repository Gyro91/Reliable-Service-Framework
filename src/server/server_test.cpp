/*
 * server.cpp
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../../include/types.hpp"
#include "../../include/server_class.hpp"
#include "../../include/test.hpp"
#include <unistd.h>


int32_t main(int32_t argc, char_t* argv[])
{	
	uint8_t service = *argv[0], id = *argv[1];
	RSF_Server *server;
	std::string broker_address("localhost");
	uint16_t broker_port = REG_PORT_BROKER;

	try {
		server = new RSF_Server(id, service, broker_address,
			broker_port);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() <<  std::endl;
		exit(EXIT_FAILURE);
	}
	
	server->step();	
	
	delete server;

	return EXIT_SUCCESS;
}
