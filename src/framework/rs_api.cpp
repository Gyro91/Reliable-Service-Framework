/*
 *	rs_api.cpp
 *	
 */
#include <new>
#include <iostream>
#include "../../include/rs_api.hpp"
#include <stdio.h>

/**
 * @brief It requests to the broker to register the server copies. 
 * @param reg_mod Registration module to forward to the broker for a request
 * @param socket socket used for the communication
 * @retval It returns the broker dealer port if the service is accepted, otherwise 0
 */

uint16_t register_service(registration_module *reg_mod, zmq::socket_t *socket)
{
	uint16_t dealer_port;
	zmq::message_t *request;
	
	try {
		request = new zmq::message_t(sizeof(registration_module));	
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	/* Sending request */
       	memcpy(request->data(), (void *) reg_mod, sizeof(registration_module));
        std::cout << "Sending a request for the service "<< reg_mod->service << std::endl;
        bool ret = socket->send(*request);
        if (ret == false) {
        	perror("Error sending \n");
        }
        std::cout << "Sended "<< std::endl;

        /* Receiving an answer */
        zmq::message_t reply;
    	socket->recv(&reply);
	dealer_port = *(static_cast<uint16_t*> (reply.data()));
	std::cout << "Received Dealer port" << dealer_port << std::endl;

	return dealer_port;
}