/*
 * client.cpp
 * Main program of the client
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <zmq.hpp>
#include "../../include/rs_api.hpp"
#include "../../include/communication.hpp"



int32_t main(int32_t argc, char_t* argv[])
{	
    	bool ret;
        int32_t result;
        request_module rm;
   	zmq::context_t context(1);
   	zmq::socket_t socket(context, ZMQ_REQ);

   	std::cout << "Connecting to the server…" << std::endl;
	socket.connect("tcp://localhost:5559");

	rm.service = INCREMENT;
	rm.parameter = 2;
	for (uint8_t i = 0; i < 5; i++) {
                ret = request_service(rm, &socket, result);
                if (ret) {
                        std::cout << "Happy:)" << std::endl;
                        std::cout << "Result " << result << std::endl;
                }
                else
                        std::cout << "Sad:(" << std::endl;
        }
        
	return EXIT_SUCCESS;
}