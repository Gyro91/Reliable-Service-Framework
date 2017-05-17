/*
 *	broker_class.cpp
 *
 */
#include <string>
#include <iostream>
#include <unistd.h>
#include "../../include/broker_class.hpp"
#include "../../include/communication.hpp"

/**
 * @brief Broker constructor that initializes alle the private data and
 * 	  claims memory for ZPQ sockets. Then it connects to the socket.
 * @param nmr Redundancy for the voter
 * @param port_router It is the port for client communication
 * @param port_reg It is the port for the server registration
 * 
 */

Broker::Broker(uint8_t nmr, uint16_t port_router, uint16_t port_reg) 
{	
	std::string conf;
	std::string protocol(COM_PROTOCOL);
	char_t str[MAX_LENGTH_STRING_PORT];

	this->nmr = nmr;
	this->port_router = port_router;
	this->port_reg = port_reg;

	/* Allocating ZMQ context */
	try {
		context = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Router socket creation */
	try {
		router = new zmq::socket_t(*context, ZMQ_ROUTER);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	memset(str, '\0', MAX_LENGTH_STRING_PORT);
	sprintf(str, "%d", port_router);
	conf = (protocol + "*" + ":" + str);
	router->bind(conf.c_str());
	std::cout << conf << std::endl;
	/* Registration socket creation */
	try {
		reg = new zmq::socket_t(*context, ZMQ_REP);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	memset(str, '\0', MAX_LENGTH_STRING_PORT);
	sprintf(str, "%d", port_reg);
	conf = (protocol + ANY_ADDRESS + ":" + str);
	router->bind(conf.c_str());

	/* Initialize the poll set */
	nitems = 2;
	zmq::pollitem_t tmp = {static_cast<void*>(*router), 0, ZMQ_POLLIN, 0};
	items.push_back(tmp);
	tmp = {static_cast<void*>(*reg), 0, ZMQ_POLLIN, 0};
	items.push_back(tmp);

}

/* Broker denstructor */

Broker::~Broker()
{
	delete context;
	delete router;
	delete reg;
}

void Broker::step()
{
	int32_t val, more, val_elab = 0;


	for (;;) {
		zmq::message_t message;

		zmq::poll(items, -1);

		if (items[0].revents & ZMQ_POLLIN) {

			for(;;) {
				/* Receiving the value to elaborate */
				router->recv(&message);
				size_t more_size = sizeof (more);
				router->getsockopt(ZMQ_RCVMORE, &more, 
					&more_size);
				
				if (more == 1) {
					router->send(message, ZMQ_SNDMORE);
				}
				//sleep(1);

				if (!more) {
					val = *(static_cast<int32_t*> 
						(message.data()));
					std::cout << "Received " 
						<< val << std::endl;

					/* Elaborating */
					val_elab = ++val;
					/* Sending back the result */
					zmq::message_t reply(4);
					memcpy(reply.data(), 
						(void *) &val_elab, 4);
					std::cout << "Sending "<< val_elab 
						<< std::endl;
					router->send(reply, 0);
					break;
				}
			}
		}
	}
}