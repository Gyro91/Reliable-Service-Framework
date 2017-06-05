/**
 * health_checker_class.cpp
 */

#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "../../include/health_checker_server_class.hpp"
#include "../../include/util.hpp"
#include "../../include/communication.hpp"

/**
 * @brief Health checker constructor
 * @param srv_pid PID of the monitored server process
 * @param srv_id ID of the monitored service redundant copy
 * @param srv_service Service provided by yhe monitored server
 * @param srv_port Port address for the server socket
 */

HealthCheckerServer::HealthCheckerServer(pid_t pid, uint16_t port, uint8_t server_id, 
	uint8_t service) : HealthChecker(pid, port)
{
	this->server_id = server_id;
	this->service = service;
}

HealthCheckerServer::~HealthCheckerServer()
{
	
}

/**
 * @brief Body of the health cheker process
 *
 */

void HealthCheckerServer::step()
{
	/* Message buffer to receive pong from the server */
	zmq::message_t buffer;
	bool pong_arrived;
	bool timeout = false;
	
	/* Send the first ping */
	buffer.rebuild((void*)& pid, sizeof(pid));
	hb_skt->send(buffer);
	pong_arrived = false;

	for (;;) {
		zmq::poll(&item, 1, HC_HEARTBEAT_INTERVAL);
		
		timeout = true;
		
		if (item.revents & ZMQ_POLLIN) {
			write_log(log_file, my_name, 
				"Received pong from server " + std::to_string(
				(int32_t)server_id));
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
				write_log(log_file, my_name,
					"Server down... Restarting");
				restart_process();
				} else
					write_log(log_file, my_name,
					"Server timeout");
		}
	}
}

void HealthCheckerServer::restart_process()
{
	int32_t ret;
	char_t server_service = static_cast<char_t>(service);
	
	/* Kill the faulty server process and start a new one */
	kill(pid, SIGKILL);
	pid = fork();
	if (pid == 0) {
		/* New server process */
		ret = execlp("./server", &server_service, &server_id,
					(char_t *)NULL);
		if (ret == -1) {
			perror("Error execlp on restarting server");
			exit(EXIT_FAILURE);
		}
	}
	write_log(log_file, my_name, "Server " + 
		std::to_string((int32_t)server_id) + " restarted, new PID: " + 
		std::to_string(pid));
}