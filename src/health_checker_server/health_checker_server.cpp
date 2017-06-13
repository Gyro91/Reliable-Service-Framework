/**
 *
 * health_checker.cpp
 * 
 */
 
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "../../include/health_checker_server_class.hpp"
#include "../../include/communication.hpp"
#include "../../include/test.hpp"


int32_t main(int32_t argc, char_t* argv[])
{	
	HealthCheckerServer* hb;
	uint16_t srv_port;
	uint8_t srv_id;
	uint8_t srv_service;
	pid_t srv_pid;
	int32_t ret;
	
	if (argc < HB_ARGS) {
		std::cerr << "Missing mandatory arguments!" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	srv_service = *argv[0];
	srv_id = *argv[1];
	srv_port = SERVER_PONG_PORT + srv_id + srv_service * MAX_NMR;
	
	/* Start the server process */
	srv_pid = fork();
	if (srv_pid == 0) {
		/* Becoming one of the redundant copies */
		ret = execlp("./RSF_server", argv[0], argv[1],
			(char_t *)NULL);
		if (ret == -1) {
			perror("Error execlp on server");
			exit(EXIT_FAILURE);
		}
	}

	/* Instanciate the health checker */
	try {
		hb = new HealthCheckerServer(srv_pid, srv_port, srv_id, 
			srv_service);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() <<  std::endl;
		exit(EXIT_FAILURE);
	}
		
	hb->step();	
	
	delete hb;

	return EXIT_SUCCESS;
}