/**
 * health_checker_class.cpp
 */

#include <iostream>
#include <stdio.h>
#include "../../include/health_checker_class.hpp"
#include "../../include/util.hpp"
#include "../../include/communication.hpp"

HealthCheker::HealthCheker(pid_t srv_pid, uint16_t srv_port)
{
	std::string srv_address("localhost");
	this->srv_pid = srv_pid;
	this->srv_port = srv_port;
	this->srv_expiry = SRV_OK;
	
	/* Allocating ZMQ context */
	try {
		ctx = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	/* Add the dealer socket */
	hb_skt = add_socket(ctx, srv_address, srv_port, ZMQ_REQ, CONNECT);
	
	/* Add the socket to the poll set */
	item = {static_cast<void*>(*hb_skt), 0, ZMQ_POLLIN, 0};
}

HealthCheker::~HealthCheker()
{
	delete hb_skt;
	delete ctx;
}

/**
 * @brief Body of the health cheker process
 */
 
void HealthCheker::step()
{
	/* Message buffer to receive pong from the server */
	zmq::message_t buffer;
	
	/* Send the initial ping */
	hb_skt->send(buffer);
	buffer.rebuild();
	
	for (;;) {
		zmq::poll(&item, 1, HEARTBEAT_INTERVALL);
		
		srv_expiry = SRV_TIMEOUT;
		
		if (item.revents & ZMQ_POLLIN) {
			hb_skt->recv(&buffer);
			srv_expiry = SRV_OK;
			
			/* Send the next ping */
			hb_skt->send(buffer);
			buffer.rebuild();
		}
		
		if (srv_expiry == SRV_TIMEOUT) {
			std::cout << "Server Timeout!!!" << std::endl;
		}
			
	}
}

