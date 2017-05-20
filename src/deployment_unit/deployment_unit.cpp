/*
 * deployment_unit.cpp
 *
 * This unit deploys a redundant server. It creates num_copy_server copies
 * of a specified server.
 *
 */

#include <iostream>
#include "../../include/util.hpp"
#include "../../include/communication.hpp"
#define NUM_OPTIONS 2
#define NUM_MIN_NMR 1

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
	std::cout << "#Deployment_Unit: Spawning server copies"
			<< std::endl;
	deployment(service, num_copy_server, list_server_pid, &status);


	return EXIT_SUCCESS;
}
