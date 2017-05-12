/*
 * server_class.hpp
 *
 */

#ifndef INCLUDE_SERVER_CLASS_HPP_
#define INCLUDE_SERVER_CLASS_HPP_

#include <zmq.hpp>
#include <string>
#include "types.hpp"

class Server {

private:
	/* Idientifier among server copies */
	uint8_t id;
	/* Variables for communication */
	std::string server_address;
	uint16_t server_port;
	std::string broker_address;
	uint16_t broker_port;
	/* Variables for zmq communication */
	zmq::context_t *context;
	zmq::socket_t *receiver;
	zmq::socket_t *sender;
public:
	Server(uint8_t id, std::string server_addr, 
		uint16_t server_port, std::string broker_addr, 
		uint16_t broker_port);
	void wait_request();
	void deliver_service();
	~Server();
};

#endif /* INCLUDE_SERVER_CLASS_HPP_ */