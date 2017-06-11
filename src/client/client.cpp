/*
 * client.cpp
 * Main program of the client
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <zmq.hpp>
#include <sstream>
#include "../../include/rsf_api.hpp"
#include "../../include/util.hpp"
#include "../../include/communication.hpp"



int32_t main(int32_t argc, char_t* argv[])
{	
	bool ret;
	int32_t result;
	service_type_t service;
	std::string addr("127.0.0.1");
	uint16_t port = 5559;
	/* Instantiate the RSF_Client object */
	RSF_Client client(addr, port);
	/* Parsing the arguments */
	get_arg(argc, argv, service, 1);


	for (uint8_t i = 0; i < 5; i++) {
                ret = client.request_service(service, result, 2);
                if (ret) 
                        std::cout << "Result " << result << std::endl;
        }
        
	return EXIT_SUCCESS;
}