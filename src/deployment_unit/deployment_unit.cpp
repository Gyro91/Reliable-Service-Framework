/*
 * deployment_unit.cpp
 *
 * This unit deploys a redundant server. It creates num_copy_server copies
 * of a specified server.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <new>
#include <iostream>
#include "../../include/check_util.hpp"
#define NUM_OPTIONS 2
#define NUM_MIN_NMR 1

int32_t main(int32_t argc, char_t* argv[])
{
	int8_t ret;
	uint8_t i;
	int32_t status = 0;
	char_t num_copy_server, service;
	pid_t *list_server_pid = NULL;

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
		std::cerr << "bad_alloc caught: " << ba.what() << '\n';
		exit(EXIT_FAILURE);
	}

	/* Spawning server copies */
	std::cout << "#Deployment_Unit: Spawning server copies"
			<< std::endl;
	for (;;) {
		if (i == num_copy_server) {
			std::cout << "#Deployment_Unit: "
					"Server copies deployed"
						<< std::endl;
			/* Registration phase */
						
			/* Wait on the children */
			wait(&status);
			std::cerr << "Wake up!Something happened to "
					"my children!" << std::endl;
			break;
		}
		list_server_pid[i] = fork();
		if (list_server_pid[i] == 0) {
			/* Becoming one of the redundant copies */
			ret = execlp("./server", &service, &i,
					(char_t *)NULL);
			if (ret == -1) {
				perror("Error execlp on server");
				exit(EXIT_FAILURE);
			}
		} else
			i++;
	}

	/* Cleaning memory */
	delete[] list_server_pid;

	return EXIT_SUCCESS;
}
