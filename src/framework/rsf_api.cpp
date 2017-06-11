/*
 *	rsf_api.cpp
 *	
 */
#include <iostream>
#include <stdio.h>
#include "../../include/rsf_api.hpp"
#include "../../include/util.hpp"

/**
 * @brief RSF_Client constructor
 * @param addr Address of the Broker
 * @param port Listening port of the Broker
 */

RSF_Client::RSF_Client(std::string addr, uint16_t port)
{
	/* Allocating ZMQ context */
	try {
		this->context = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	/* Client socket creation */
	std::cout << "RSF_Client: Connecting to the Broker..." << std::endl;
	this->socket = add_socket(context, addr, port, ZMQ_REQ, CONNECT);
}

/**
 * @brief RSF_Client object destructor
 * @return 
 */

RSF_Client::~RSF_Client()
{
	delete socket;
	delete context;
}

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
	}

        /* Receiving an answer */
        zmq::message_t reply;
    	ret = socket->recv(&reply);
	if (ret == false) 
		return -1;
		
	send_reg = false;
	dealer_port = *(static_cast<uint16_t*> (reply.data()));
	
	return dealer_port;		
}