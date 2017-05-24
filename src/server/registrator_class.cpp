/*
 *	registrator_class.cpp
 *
 */

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../../include/registrator_class.hpp"
#include "../../include/rs_api.hpp"
#include "../../include/communication.hpp"
#include "../../include/util.hpp"

/**
 * @brief DeploymentUnit constructor that initializes all the private data 
 * @param service Service type of the server copies
 * @param reg_port Broker port for registering server copies
 * 
 */

Registrator::Registrator(std::string broker_address, service_type_t service, 
	uint16_t reg_port, zmq::context_t *ctx)
{
	this->broker_address = broker_address;
	this->service = service;
	this->reg_port = reg_port;

	/* Create the ZMQ Socket to register the service */
	reg = add_socket(ctx, broker_address, reg_port, ZMQ_REQ, CONNECT);
}

/**
 * @brief DeploymentUnit denstructor to destroy all the dynamic objects
 * 
 */

Registrator::~Registrator()
{
	/* Cleaning memory */
	delete reg;
}

/**
 * @brief This function registers the server 
 * 
 */

uint16_t Registrator::registration()
{	
	uint16_t dealer_port;
	registration_module rm;

	/* Signing the registration module */
	rm.service = service;
	memset(rm.signature, '\0', sizeof(rm.signature));
	strcpy(rm.signature, "pippo");

	/* Registering */
	dealer_port = register_service(&rm, reg);

	return dealer_port;
}



