/**
 * health_checker_class.cpp
 */

#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "../../include/health_checker_broker_class.hpp"
#include "../../include/util.hpp"
#include "../../include/communication.hpp"

/**
 * @brief Health checker constructor
 * @param pid PID of the monitored broker process
 * @param port Port address for the broker socket
 */

HealthCheckerBroker::HealthCheckerBroker(pid_t pid, uint16_t port) 
	: HealthChecker(pid, port)
{
	this->my_name = "HC_Broker";
}

HealthCheckerBroker::~HealthCheckerBroker()
{
	
}

/**
 * @brief Body of the health cheker process
 *
 */

void HealthCheckerBroker::step()
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
		zmq::poll(&item, 1, HEARTBEAT_INTERVAL);
		
		timeout = true;
		
		if (item.revents & ZMQ_POLLIN) {
			write_log(my_name,
				"Received pong from broker");
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
			} else if (--hb_liveness == 0) {
				hb_liveness = HEARTBEAT_LIVENESS;
				write_log(my_name,
				          "Broker down... Restarting");
				restart_process();
			} else
				write_log(my_name,
				          "Broker timeout");
		}
	}
}

void HealthCheckerBroker::restart_process()
{
	int8_t ret;
	char_t name[7] = "broker";
	/* Kill the faulty server process and start a new one */
	kill(pid, SIGKILL);
	pid = fork();
	if (pid == 0) {
		/* New server process */
		ret = execlp("./RSF_broker", name, (char_t *) NULL);
		if (ret == -1) {
			perror("Error execlp on restarting broker");
			exit(EXIT_FAILURE);
		}
	}
	write_log(my_name, "Broker restarted, new PID: " + 
		std::to_string(pid));
}


