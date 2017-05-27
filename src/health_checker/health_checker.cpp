/**
 *
 * health_checker.cpp
 * 
 */
 
#include <stdlib.h>
#include <iostream>
#include "../../include/health_checker_class.hpp"

#define HB_ARGS 4

int32_t main(int32_t argc, char_t* argv[])
{	
	HealthCheker* hb;
	std::string srv_address;("localhost");
	pid_t srv_pid;
	uint16_t srv_port;
	
	if (argc < HB_ARGS) {
		std::cerr << "Missing mandatory arguments!" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	srv_pid = *argv[0];
	srv_port = *argv[1];

	/* Instanciate the health checker */
	try {
		hb = new HealthCheker(srv_pid, srv_port);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() <<  std::endl;
		exit(EXIT_FAILURE);
	}
	
	std::cout <<"Server PID: " << (int32_t)srv_pid << std::endl << 
		" Server port: " << (int32_t)srv_port << std::endl <<
		"Liveness: " << HEARTBEAT_LIVENESS << std:: endl <<
		"Interval: " << HEARTBEAT_INTERVALL << std::endl;
		
	hb->step();	
	
	delete hb;

	return EXIT_SUCCESS;
}