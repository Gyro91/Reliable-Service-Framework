/*
 * deployment_unit.cpp
 *
 * This unit deploys a redundant server. It creates num_copy_server copies
 * of a specified server.
 *
 */
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include "../../include/util.hpp"
#include "../../include/communication.hpp"
#define NUM_OPTIONS 2
#define NUM_MIN_NMR 1


/**
 * @brief It deploys the server copies for the specified service
 * @param service service It is the server service
 * @param num_copy_server list_server_pid Array of server pid
 * @param list_server_pid list of the servers PIDs
 * @param status Variable to monitor children
 */

void deployment(uint8_t service, uint8_t num_copy_server, 
	pid_t *list_server_pid, int32_t *status)
{
	int8_t ret;
	uint8_t i = 0;
	pid_t hc_pid;
	char_t server_service = static_cast<char_t>(service);

	/* Server copies deployment */
	for (;;) {
		if (i == num_copy_server) {
			/* Wait on the children */
			while (true)
				wait(&status);
			std::cerr << "Wake up!Something happened to "
				"my children!" << std::endl;
			break;
		}
		/* Start the health checker process */
		hc_pid = fork();
		if (hc_pid == 0) {
			ret = execlp("./RSF_start_server", &server_service,
				&i, (char_t *)NULL);
			if (ret == -1) {
				perror("Error execlp on healt_checker_server");
				exit(EXIT_FAILURE);
			}
		} else
			i++;
	}
}

int32_t main(int32_t argc, char_t* argv[])
{
	uint8_t num_copy_server, service;
	int32_t status = 0;
	pid_t *list_server_pid;

	/* Parsing the arguments */
	get_arg(argc, argv, num_copy_server, service, NUM_OPTIONS);
	if (num_copy_server < NUM_MIN_NMR) {
		std::cerr << "Error: the server copies must be greater"
				" than " << NUM_MIN_NMR << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Allocating memory for the pid of each server */
	try {
		list_server_pid = new pid_t[num_copy_server];
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Spawning server copies */
	deployment(service, num_copy_server, list_server_pid, &status);

	
	return EXIT_SUCCESS;
}
