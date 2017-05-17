/*
 *	deployment_unit_class.cpp
 *
 */
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <new>
#include "../../include/deployment_unit_class.hpp"

/**
 * @brief DeploymentUnit constructor that initializes alle the private data 
 * @param num_copy_server Number of server copies to be deployed
 * @param service Service type of the server copies
 * @param reg_port Broker port for registering server copies
 * 
 */

DeploymentUnit::DeploymentUnit(uint8_t num_copy_server, service_type_t service, 
		uint16_t reg_port)
{
	this->num_copy_server = num_copy_server;
	this->service = service;
	this->reg_port = reg_port;

	/* Allocating memory for the pid of each server */
	try {
		list_server_pid = new pid_t[num_copy_server];
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief DeploymentUnit denstructor to destroy all the dynamic objects
 * 
 */

DeploymentUnit::~DeploymentUnit()
{
	/* Cleaning memory */
	delete[] list_server_pid;
}

/**
 * @brief This function deploys the server copies
 * 
 */

void DeploymentUnit::deployment()
{	
	int8_t ret;
	uint8_t i = 0;
	int32_t status = 0;

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
			char_t server_service = service;
			/* Becoming one of the redundant copies */
			ret = execlp("./server", &server_service, &i,
					(char_t *)NULL);
			if (ret == -1) {
				perror("Error execlp on server");
				exit(EXIT_FAILURE);
			}
		} else
			i++;
	}
}

