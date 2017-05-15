/*
 *	broker_class.cpp
 *
 */
#include <string>
#include <iostream>
#include <unistd.h>
#include "../../include/broker_class.hpp"
#include "../../include/communication.hpp"

void set_pollitem(zmq::pollitem_t *p_item, zmq::socket_t *skt)
{
	p_item->socket = static_cast<void*> (skt);
	p_item->fd = 0;
	p_item->events = ZMQ_POLLIN;
	p_item->revents = 0;
}

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
	zmq::pollitem_t tmp = {static_cast<void*>(router), 0, ZMQ_POLLIN, 0};
	items.push_back(tmp);
	tmp = {static_cast<void*>(reg), 0, ZMQ_POLLIN, 0};
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
	int32_t val, val_elab;
	bool ret;
	zmq::pollitem_t *itemz = static_cast<zmq::pollitem_t *> (items.data());

	for (;;) {
		zmq::message_t message;

		ret = zmq::poll(itemz, nitems, -1);
		if (ret < 0) {
			perror("Error poll");
			exit(EXIT_FAILURE);
		}
		if (itemz[0].revents & ZMQ_POLLIN) {
			/* Receiving the value to elaborate */
	       		ret = router->recv(&message);
	       		if (ret == true) {
	       			val = *(static_cast<int32_t*> (message.data()));
	            		std::cout << "Received " << val << std::endl;
	       		}
       			/* Elaborating */
       			val_elab = ++val;
       			sleep(1);

       			/* Sending back the result */
       			zmq::message_t reply(4);
       			memcpy(reply.data(), (void *) &val_elab, 4);
	        	std::cout << "Sending "<< val_elab << std::endl;
	       	   	router->send(reply);
		}
	}
}