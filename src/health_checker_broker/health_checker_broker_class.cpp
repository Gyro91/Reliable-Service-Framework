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
			std::cout << "HC: Received pong from broker" << 
				std::endl;
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
				std::cout << "Broker down... restarting" <<
					std::endl;
				restart_process();
				} else
					std::cout << "Broker Timeout!" <<
						std::endl;
		}
	}
}

void HealthCheckerBroker::restart_process()
{
	int8_t ret;
	char_t name[7] = "broker";
	/* Kill the faulty server process and start a new one */
//	ret = kill(srv_pid, SIGKILL);
//	if (ret != 0) {
//		std::cout << "Error during kill!" << std::endl;
//		exit(EXIT_FAILURE);
//	}
	pid = fork();
	if (pid == 0) {
		/* New server process */
		ret = execlp("./broker", name, (char_t *) NULL);
		if (ret == -1) {
			perror("Error execlp on restarting broker");
			exit(EXIT_FAILURE);
		}
	}

}


