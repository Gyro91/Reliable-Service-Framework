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
#include "service_database_class.hpp"

class Broker {

private:
	/* Redundancy for the voter */
	uint8_t nmr;
	/* Ports for communication */
	std::list<uint16_t> port_dealer; 
	uint16_t port_router;
	uint16_t port_reg;
	uint16_t available_dealer_port;
	/* Poll set */
	std::vector<zmq::pollitem_t> items;
	/* Sockets for ZMQ communication */
	zmq::context_t *context;
	std::vector<zmq::socket_t*> dealer;
	zmq::socket_t *reg;
	zmq::socket_t *router;
	/* Services Database */
	ServiceDatabase *db;

	/* Function for voting */
	uint8_t vote(int32_t values[]);
	/* Function for adding a dealer socket */
	void add_dealer(uint16_t dealer_port);	
public:
	Broker(uint8_t nmr, uint16_t port_router, uint16_t port_reg);
	void step();
	~Broker();
};

#endif /* INCLUDE_BROKER_CLASS_HPP_ */