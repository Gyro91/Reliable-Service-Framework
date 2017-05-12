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
	int service = *argv[0], id = *argv[1];

	std::cout << id << std::endl;

	Server *server = new Server(id, "127.0.0.1", 5000,
				"127.0.0.1", 5000);

	server->wait_request();				
	


	return EXIT_SUCCESS;
}
