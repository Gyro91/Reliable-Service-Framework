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

HealthChecker::HealthChecker(pid_t srv_pid, uint8_t srv_id, uint8_t srv_service, 
	uint16_t srv_port)
{
	this->srv_pid = srv_pid;
	this->srv_id = srv_id;
	this->srv_service = srv_service;
	this->srv_port = srv_port;
	this->hb_liveness = HEARTBEAT_LIVENESS;
	
	/* Allocating ZMQ context */
	context_init();
}

/**
 * @brief Health checker destructor
 */

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
	bool pong_arrived;
	bool timeout = false;
	
	/* Send the first ping */
	buffer.rebuild((void*)&srv_pid, sizeof(srv_pid));
	hb_skt->send(buffer);
	pong_arrived = false;

	for (;;) {
		zmq::poll(&item, 1, HEARTBEAT_INTERVAL);
		
		timeout = true;
		
		if (item.revents & ZMQ_POLLIN) {
			std::cout << "HC: Received pong from server " << 
				(uint32_t) srv_id << std::endl;
			hb_skt->recv(&buffer);
			hb_liveness = HEARTBEAT_LIVENESS;
			timeout = false;
			pong_arrived = true;
		}
		
		if (timeout) {
			
			/* Send the next ping */
			if (pong_arrived) {
				buffer.rebuild(EMPTY_MSG, 0);
				hb_skt->send(buffer);
				pong_arrived = false;
			} else if (--hb_liveness == 0){
				hb_liveness = HEARTBEAT_LIVENESS;
				std::cout << "Server down... restarting" <<
					std::endl;
				restart_server();
				} else
					std::cout << "Server Timeout!!!" <<
						std::endl;
		}
	}
}

void HealthChecker::restart_server()
{
	int8_t ret;
	char_t server_service = static_cast<char_t>(srv_service);
	/* Delete the actual ZMQ context */
	delete hb_skt;
	delete ctx;
	
	/* Kill the faulty server process and start a new one */
	ret = kill(srv_pid, SIGKILL);
	if (ret != 0) {
		std::cout << "Error during kill!!!" << std::endl;
		exit(EXIT_FAILURE);
	}
	srv_pid = fork();
	if (srv_pid == 0) {
		/* New server process */
		ret = execlp("./server", &server_service, &srv_id,
					(char_t *)NULL);
		if (ret == -1) {
			perror("Error execlp on restarting server");
			exit(EXIT_FAILURE);
		}
	}
	/* Initialize a new ZMQ context */
	context_init();
}

/**
 * @brief Allocates the ZMQ context, adds the needed socket and the poll item.
 */

void HealthChecker::context_init()
{
	std::string srv_address("localhost");
	
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

