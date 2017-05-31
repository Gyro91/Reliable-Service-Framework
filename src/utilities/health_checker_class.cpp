/**
 * health_checker_class.cpp
 */

#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
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

HealthChecker::HealthChecker(pid_t pid, uint16_t port)
{
	this->pid = pid;
	this->port = port;
	this->hb_liveness = HEARTBEAT_LIVENESS;
	
	/* Allocating ZMQ context */
	context_init();
}

HealthChecker::~HealthChecker()
{
	delete hb_skt;
	delete ctx;
}

/**
 * @brief Allocates the ZMQ context, adds the needed socket and the poll item.
 */

void HealthChecker::context_init()
{
	std::string address("localhost");
	
	try {
		ctx = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	/* Add the dealer socket */
	hb_skt = add_socket(ctx, address, port, ZMQ_REQ, CONNECT);
	
	/* Add the socket to the poll set */
	item = {static_cast<void*>(*hb_skt), 0, ZMQ_POLLIN, 0};
}

