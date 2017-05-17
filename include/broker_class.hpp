/*
 * broker_class.hpp
 *
 */

#ifndef INCLUDE_BROKER_CLASS_HPP_
#define INCLUDE_BROKER_CLASS_HPP_

#include <zmq.hpp>
#include <string>
#include <unistd.h>
#include <list>
#include "types.hpp"
#include "service.hpp"

class Broker {

private:
	uint8_t nmr;
	/* Ports for communication */
	std::list<uint16_t> port_dealer; 
	uint16_t port_router;
	uint16_t port_reg;
	/* Poll set */
	uint16_t nitems;	
public:
	std::vector<zmq::pollitem_t> items;
	/* Sockets for ZMQ communication */
	zmq::context_t *context;
	zmq::socket_t **dealer;
	zmq::socket_t *reg;
	zmq::socket_t *router;
	Broker(uint8_t nmr, uint16_t port_router, uint16_t port_reg);
	void step();
	~Broker();
};

#endif /* INCLUDE_BROKER_CLASS_HPP_ */