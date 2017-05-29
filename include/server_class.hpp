/*
 *	server_class.hpp
 *
 */

#ifndef INCLUDE_SERVER_CLASS_HPP_
#define INCLUDE_SERVER_CLASS_HPP_

#include <zmq.hpp>
#include <string>
#include <unistd.h>
#include "types.hpp"
#include "service.hpp"
#include "registrator_class.hpp"

#define SERVICE_REQUEST_INDEX 0
#define SERVER_PONG_INDEX 1

class Server {

private:
	/* Idientifier among server copies */
	uint8_t id;
	/* Service type that must be provided */
	service_type_t service_type;
	/* Service to be provided */
	service_body service;
	/* Address and port for communication */
	std::string broker_address;
	uint16_t broker_port;
	/* Sockets for ZMQ communication */
	zmq::context_t *context;
	zmq::socket_t *reply;
	zmq::socket_t *hc_pong;
	/* Registrator to register this unit to the broker */
	Registrator *registrator;
	/* Poll set */
	std::vector<zmq::pollitem_t> items;
	
	/* Receive requests from the broker */
	bool receive_request(int32_t *val);
	/* Send results to the broker */
	void deliver_service(int32_t val);
	/* Send a pong to the broker */
	void pong_broker();
	/* Receive the ping and send back a pong to the health checker */
	void pong_health_checker();

public:
	Server(uint8_t id, uint8_t service, std::string broker_addr, 
		uint16_t broker_port);
	void step();
	~Server();
};

#endif /* INCLUDE_SERVER_CLASS_HPP_ */