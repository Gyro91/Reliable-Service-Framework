/*
 *	server_class.hpp
 *
 */

#ifndef INCLUDE_SERVER_CLASS_HPP_
#define INCLUDE_SERVER_CLASS_HPP_

#include <zmq.hpp>
#include <string>
#include <sstream>
#include <unistd.h>
#include <functional>
#include "types.hpp"
#include "service.hpp"
#include "registrator_class.hpp"

#define SERVICE_REQUEST_INDEX 1
#define REGISTRATION_INDEX 1
#define SERVER_PONG_INDEX 0

struct service_thread_t {
	std::string parameters;
	service_body service;
	service_type_t service_type;
	uint8_t id;
	zmq::socket_t *skt;
};

class RSF_Server {

private:
	/* Idientifier among server copies */
	uint8_t id;
	/* Service type that must be provided */
	service_type_t service_type;
	/* Service to be provided */
	service_body service;
	/* Ping seq id */
	uint32_t ping_id;
	/* Ping request id */
	uint32_t request_id;
	/* Address and port for communication */
	std::string broker_address;
	int32_t broker_port;
	/* Sockets for ZMQ communication */
	zmq::context_t *context;
	zmq::socket_t *reply;
	zmq::socket_t *hc_pong;
	service_thread_t service_thread;
	/* Registrator to register this unit to the broker */
	Registrator *registrator;
	/* Poll set */
	std::vector<zmq::pollitem_t> items;
	/* Identificator used for logging */
	std::string my_name;
	
	/* Receive requests from the broker */
	bool receive_request(char_t *val, uint32_t *received_id);
	/* Send a pong to the broker */
	void pong_broker();
	/* Receive the ping and send back a pong to the health checker */
	void pong_health_checker();
	/* Function for creating a thread that elaborates a request */
	void create_thread(std::string parameter);

public:
	RSF_Server(uint8_t id, uint8_t service, std::string broker_addr,
		uint16_t broker_port);
	void step();
	~RSF_Server();
};

#endif /* INCLUDE_SERVER_CLASS_HPP_ */