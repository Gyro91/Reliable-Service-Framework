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
        std::cout << "Registration for the service " << reg_mod->service 
        	<< std::endl;
	if (!send_reg) {
		socket->send(request);
		send_reg = true;
		std::cout<<"Send"<<std::endl;
	}

        /* Receiving an answer */
        zmq::message_t reply;
    	ret = socket->recv(&reply);
	if (ret == false) {
		std::cout << "Timeout registration expired" << std::endl;
		return -1;
		
	}
	send_reg = false;
	dealer_port = *(static_cast<uint16_t*> (reply.data()));
	std::cout << "Received Dealer port " << dealer_port << std::endl;
	
	return dealer_port;		
}


/**
 * @brief      It requests a service to the broker.
 *
 * @param      rm      The request module
 * @param      socket  The socket
 * @param      result  The service result
 * 
 * @return     It returns true if the service was proviced correctly, otherwise
 * 	       false
 */
bool request_service(request_module &rm, zmq::socket_t *socket, int32_t &result) 
{
	response_module response;

	/* Service Request */
	zmq::message_t request(sizeof(request_module));
       	memcpy(request.data(), (void *) &rm, sizeof(request_module));
        std::cout << "Request for service "<< rm.service << std::endl;
        socket->send(request);
        std::cout << "Sended request"<< std::endl;
        
        /* Get response module */
        zmq::message_t reply;
    	socket->recv(&reply);
        response = *(static_cast<response_module*> (reply.data()));
        std::cout << "Status " << response.service_status << std::endl;
        if (response.service_status == SERVICE_NOT_RELIABLE || 
        	response.service_status ==  SERVICE_NOT_AVAILABLE)
        	return false;
        
        result = response.result;

        return true;
}