/**
 *
 * health_checker_broker.cpp
 * 
 */
 
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "../../include/health_checker_broker_class.hpp"
#include "../../include/communication.hpp"

#define HB_ARGS 3

int32_t main(int32_t argc, char_t* argv[])
{	
	int32_t ret;
	HealthCheckerBroker* hcb;
	char_t name[7] = "broker";
	pid_t pid;
	
	/* Creating the Broker */
	pid = fork();
	if (pid == 0) {
		/* New server process */
		ret = execlp("./broker", name, (char_t *) NULL);
		if (ret == -1) {
			perror("Error execlp on restarting broker");
			exit(EXIT_FAILURE);
		}
	}
	
	/* Instanciate the health checker */
	try {
		hcb = new HealthCheckerBroker(pid, BROKER_PONG_PORT);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() <<  std::endl;
		exit(EXIT_FAILURE);
	}
	
	std::cout <<"Broker PID: " << pid << std::endl;
		
	hcb->step();	
	
	delete hcb;

	return EXIT_SUCCESS;
}