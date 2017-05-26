/**
 * health_checker_class.cpp
 */

#include <iostream>
#include <stdio.h>
#include "health_checker_class.hpp"
#include "../../include/communication.hpp";

HealthCheker::HealthCheker(pid_t srv_pid, uint16_t srv_port, 
	uint8_t hb_liveness = 3, int64_t hb_interval = 1000)
{
	this->srv_pid = srv_pid;
	this->srv_port = srv_port;
	this->hb_liveness = hb_liveness;
	this->hb_interval = hb_interval;
	
	/* Allocating ZMQ context */
	try {
		ctx = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	/* Add the dealer socket */
	dealer = add_socket(ctx, srv_address, srv_port, ZMQ_DEALER, CONNECT);
	
	/* Add the socket to the poll set */
	item = {static_cast<void*>(*dealer), 0, ZMQ_POLLIN, 0};
}

HealthCheker::~HealthCheker()
{
	delete dealer;
	delete ctx;
}

