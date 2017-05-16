/*
 * client.cpp
 * Main program of the client
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../../include/types.hpp"
#include <zmq.hpp>


int32_t main(int32_t argc, char_t* argv[])
{	
        int32_t result;
        zmq::context_t context(1);
    	zmq::socket_t socket(context, ZMQ_REQ);

   	std::cout << "Connecting to the server…" << std::endl;
	socket.connect("tcp://localhost:5559");


	for (int32_t i = 0; i < 10; i++) {

        	zmq::message_t request(4);
       		memcpy(request.data(), (void *) &i, 4);
        	std::cout << "Sending "<< i << std::endl;
        	socket.send(request);
                std::cout << "Sended "<< i << std::endl;
        	//  Get the reply.
        	zmq::message_t reply;
    		socket.recv(&reply);
                result = *(static_cast<int32_t*> (reply.data()));
                std::cout << "Received " << result << std::endl;
        }

	return EXIT_SUCCESS;
}