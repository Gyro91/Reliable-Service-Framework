/*
 * deployment_unit_class.hpp
 *
 */

#ifndef INCLUDE_DEPLOYMENT_UNIT_CLASS_HPP_
#define INCLUDE_DEPLOYMENT_UNIT_CLASS_HPP_

#include <zmq.hpp>
#include "types.hpp"
#include "service.hpp"

class DeploymentUnit {

private:
	/* Number of server copies to be deployed */
	uint8_t num_copy_server;
	/* Type of server service */
	service_type_t service;
	/* Broker Port for communication between broker and servers */
	uint16_t dealer_port;
	/* Broker Port for registering server copies */
	uint16_t reg_port;
	/* Array of servers pid */
	pid_t *list_server_pid;
public:
	/* Sockets for ZMQ communication */
	zmq::context_t *context;
	zmq::socket_t *reg;

	DeploymentUnit(uint8_t num_copy_server, service_type_t service, 
		uint16_t reg_port);
	void deployment();
	~DeploymentUnit();
};

#endif /* INCLUDE_DEPLOYMENT_UNIT_CLASS_HPP_ */