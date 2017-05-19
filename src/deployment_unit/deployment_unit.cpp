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
#include "../../include/deployment_unit_class.hpp"
#define NUM_OPTIONS 2
#define NUM_MIN_NMR 1

int32_t main(int32_t argc, char_t* argv[])
{
	uint8_t num_copy_server;
	service_type_t service;
	DeploymentUnit *deployment_unit;

	/* Parsing the arguments */
	get_arg(argc, argv, num_copy_server, service, NUM_OPTIONS);
	if (num_copy_server < NUM_MIN_NMR) {
		std::cerr << "Error: the server copies must be greater"
				" than " << NUM_MIN_NMR << std::endl;
		exit(EXIT_FAILURE);
	}

	try {
		deployment_unit = new DeploymentUnit(LOCALHOST, num_copy_server,
			service, REG_PORT_BROKER);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	/* Registering Server */
	//deployment_unit->registration();
	/* Spawning server copies */
	std::cout << "#Deployment_Unit: Spawning server copies"
			<< std::endl;
	
	deployment_unit->deployment();


	return EXIT_SUCCESS;
}
