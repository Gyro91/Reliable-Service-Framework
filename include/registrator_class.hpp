/*
 * registrator_class.hpp
 *
 */

#ifndef INCLUDE_REGISTRATOR_CLASS_HPP_
#define INCLUDE_REGISTRATOR_CLASS_HPP_

#include <zmq.hpp>
#include <string>
#include "types.hpp"
#include "service.hpp"

class Registrator {

private:
	/* Type of server service */
	service_type_t service;
	/* Broker Address */
	std::string broker_address;
	/* Broker Port for registering server copies */
	uint16_t reg_port;
public:
	zmq::socket_t *reg;

	Registrator(std::string broker_address, service_type_t service, 
		uint16_t reg_port, zmq::context_t *ctx);
	uint16_t registration();
	~Registrator();
};

#endif /* INCLUDE_REGISTRATOR_HPP_ */