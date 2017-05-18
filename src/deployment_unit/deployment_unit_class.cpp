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
#include "../../include/rs_api.hpp"
#include "../../include/communication.hpp"

/**
 * @brief DeploymentUnit constructor that initializes all the private data 
 * @param num_copy_server Number of server copies to be deployed
 * @param service Service type of the server copies
 * @param reg_port Broker port for registering server copies
 * 
 */

DeploymentUnit::DeploymentUnit(std::string broker_address, uint8_t num_copy_server, service_type_t service, uint16_t reg_port)
{	
	std::string conf;
	std::string protocol(COM_PROTOCOL);
	char_t str[MAX_LENGTH_STRING_PORT];
	
	this->broker_address = broker_address;
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

	/* Allocating ZMQ context */
	try {
		context = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Preparing ZMQ Socket to register the service */
	try {
		reg = new zmq::socket_t(*context, ZMQ_REQ);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	memset(str, '\0', MAX_LENGTH_STRING_PORT);
	sprintf(str, "%d", reg_port);
	conf = (protocol + broker_address + ":" + str);
	reg->connect(conf.c_str());

	std::cout << conf << std::endl;
}

/**
 * @brief DeploymentUnit denstructor to destroy all the dynamic objects
 * 
 */

DeploymentUnit::~DeploymentUnit()
{
	/* Cleaning memory */
	delete[] list_server_pid;
	delete context;
	delete reg;
}

/**
 * @brief This function registers the server copies
 * 
 */

void DeploymentUnit::registration()
{
	registration_module rm;

	rm.service = service;

	register_service(&rm, reg);
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

	/* Server copies deployment */
	for (;;) {
		if (i == num_copy_server) {
			std::cout << "#Deployment_Unit: "
					"Server copies deployed"
						<< std::endl;	
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

