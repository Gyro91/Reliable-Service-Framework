/**
 *	server_class.cpp
 * 
 */
#include <iostream>
#include <stdio.h>
#include "../../include/server_class.hpp"
#include "../../include/communication.hpp"
#include "../../include/util.hpp"

#define MAX_LENGTH_STRING_PORT 6 /* Max number of char needed for data port */

/**
 * @brief Server constructor that initializes alle the private data and
 * 	  claims memory for ZPQ sockets. Then it connects to the socket.
 * @param id_server Identifier among server copies
 * @param service_t Service type to be deployed
 * @param server_p Server receive port
 * @param broker_addr Broker address
 * @param broker_p Broker port
 * 
 */

Server::Server(uint8_t id_server, uint8_t service_t, std::string broker_addr, 
		uint16_t broker_p) 
{
	id = id_server;
	service_type = (service_type_t)service_t;
	broker_port = broker_p;
	broker_address = broker_addr;

	service = get_service_body(service_type);

	/* Allocating ZMQ context */
	try {
		context = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	/* In this case the REP socket requires the connect() method! */
	reply = add_socket(context, broker_addr, broker_p, ZMQ_REP, CONNECT);
}

/**
 * @brief Server denstructor to destroy all the dynamic objects
 * 
 */

Server::~Server()
{
	delete context;
	delete reply;
}

/**
 * @brief Server step function used to perform all the computation.
 */

void Server::step()
{	 
	int32_t val, val_elab;
	try {
		for (;;) {
			/* Receiving the value to elaborate */
			receive_request(&val);

			/* Elaborating */
			val_elab = service(val);
			sleep(1);

			/* Sending back the result */
			deliver_service(val_elab);
		}
	} catch (std::exception &e) {}
}

/**
 * @brief Receive the request message from the broker and returns 
 * 	  the data contained inside it.
 * @param val	Reference to return the message data.
 */

void Server::receive_request(int32_t* val)
{
	zmq::message_t msg;
	bool ret;
	
	ret = reply->recv(&msg);
	if (ret == true) {
		*val = *(static_cast<int32_t*> (msg.data()));
		std::cout << "Received " << val << std::endl;
	} else {
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Sends back to the broker the results of the service.
 * @param val	Service result to be sent.
 */

void Server::deliver_service(int32_t val)
{
	zmq::message_t msg(4);
	bool ret;
	
	memcpy(msg.data(), (void *) &val, 4);
	std::cout << "Sending " << val << std::endl;
	
	ret = reply->send(msg);
	if (ret == true)
		std::cout << "Sent " << val << std::endl;
	else
		exit(EXIT_FAILURE);
}