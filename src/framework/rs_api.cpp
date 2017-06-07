/*
 *	rs_api.cpp
 *	
 */
#include <new>
#include <iostream>
#include "../../include/rs_api.hpp"
#include <stdio.h>
#include <tuple>

/**
 * @brief It requests to the broker to register the server copies. 
 * @param reg_mod Registration module to forward to the broker for a request
 * @param socket socket used for the communication
 * @retval It returns the broker dealer port if the service is accepted, 
 * 	   otherwise 0
 */

int32_t register_service(registration_module *reg_mod, zmq::socket_t *socket)
{
	uint16_t dealer_port;
	static bool send_reg = false;
	bool ret;
	zmq::message_t request(sizeof(registration_module));
	
	/* Sending request */
       	memcpy(request.data(), (void *) reg_mod, sizeof(registration_module));
	if (!send_reg) {
		socket->send(request);
		send_reg = true;
		std::cout<<"Send"<<std::endl;
	}

        /* Receiving an answer */
        zmq::message_t reply;
    	ret = socket->recv(&reply);
	if (ret == false) 
		return -1;
		
	send_reg = false;
	dealer_port = *(static_cast<uint16_t*> (reply.data()));
	std::cout << "Received Dealer port " << dealer_port << std::endl;
	
	return dealer_port;		
}