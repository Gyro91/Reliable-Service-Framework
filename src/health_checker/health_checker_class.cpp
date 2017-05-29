/**
 * health_checker_class.cpp
 */

#include <iostream>
#include <stdio.h>
#include "../../include/health_checker_class.hpp"
#include "../../include/util.hpp"
#include "../../include/communication.hpp"

/**
 * @brief Health checker constructor
 * @param srv_pid PID of the monitored server process
 * @param srv_id ID of the monitored service redundant copy
 * @param srv_service Service provided by yhe monitored server
 * @param srv_port Port address for the server socket
 */

HealthChecker::HealthChecker(pid_t srv_pid, uint8_t srv_id, uint8_t srv_service, 
	uint16_t srv_port)
{
	std::string srv_address("localhost");
	this->srv_pid = srv_pid;
	this->srv_id = srv_id;
	this->srv_service = srv_service;
	this->srv_port = srv_port;
	this->srv_expiry = SRV_OK;
	this->hb_liveness = HEARTBEAT_LIVENESS;
	
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

HealthChecker::~HealthChecker()
{
	delete hb_skt;
	delete ctx;
}

/**
 * @brief Body of the health cheker process
 */
 
void HealthChecker::step()
{
	/* Message buffer to receive pong from the server */
	zmq::message_t buffer;
	bool ping_sent;
	
	/* Send the first ping */
	buffer.rebuild((void*)&srv_pid, sizeof(srv_pid));
	hb_skt->send(buffer);
	ping_sent = true;
	
	for (;;) {

		zmq::poll(&item, 1, HEARTBEAT_INTERVAL);
		
		srv_expiry = SRV_TIMEOUT;
		
		if (item.revents & ZMQ_POLLIN) {
			std::cout << "HC: Received pong from server " << 
				(uint32_t) srv_id << std::endl;
			hb_skt->recv(&buffer);
			srv_expiry = SRV_OK;
			hb_liveness = HEARTBEAT_LIVENESS;
			ping_sent = false;
		}
		
		if (srv_expiry == SRV_TIMEOUT) {
			std::cout << "Server Timeout!!!" <<
				(int32_t)hb_liveness << std::endl;
			/* Send the next ping */
			if (!ping_sent) {
				buffer.rebuild(EMPTY_MSG, 0);
				hb_skt->send(buffer);
				ping_sent = true;
			}
			if (--hb_liveness == 0) {
				hb_liveness = HEARTBEAT_LIVENESS;
				std::cout << "Server down... restarting" <<
					std::endl;
			}
		}

	}
}

