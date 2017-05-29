/**
 *
 * health_checker.cpp
 * 
 */
 
#include <stdlib.h>
#include <iostream>
#include "../../include/health_checker_class.hpp"
#include "../../include/communication.hpp"

#define HB_ARGS 3

int32_t main(int32_t argc, char_t* argv[])
{	
	HealthCheker* hb;
	pid_t srv_pid;
	uint16_t srv_port;
	uint8_t srv_id;
	uint8_t srv_service;
	
	if (argc < HB_ARGS) {
		std::cerr << "Missing mandatory arguments!" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	srv_id = atoi(argv[0]);
	srv_service = atoi(argv[1]);
	srv_pid = atoi(argv[2]);
	srv_port = SERVER_PONG_PORT + srv_id;

	/* Instanciate the health checker */
	try {
		hb = new HealthCheker(srv_pid, srv_id, srv_service, srv_port);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() <<  std::endl;
		exit(EXIT_FAILURE);
	}
	
	std::cout <<"Server PID: " << srv_pid << " Server port: " << 
		(int32_t)srv_port << " Liveness: " << HEARTBEAT_LIVENESS << 
		std::endl << " Interval: " << HEARTBEAT_INTERVAL << std::endl;
		
	hb->step();	
	
	delete hb;

	return EXIT_SUCCESS;
}