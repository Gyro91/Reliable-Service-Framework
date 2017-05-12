#include "../../include/server_class.hpp"
#include "../../include/communication.hpp"
#include <iostream>
#include <stdio.h>

#define MAX_LENGTH_STRING_PORT 6 /* Max number of char needed for data port */

/**
 * @brief Server constructor that initializes alle the private data and
 * 	  claims memory for ZPQ sockets. Then it connects to the socket.
 * @param id_server Identifier among server copies
 * @param service Service type to be deployed
 * @param server_p Server receive port
 * @param broker_addr Broker address
 * @param broker_p Broker port
 * 
 */

Server::Server(uint8_t id_server, uint8_t service, std::string server_addr, 
		uint16_t server_p, std::string broker_addr, uint16_t broker_p) 
{
	char_t str[MAX_LENGTH_STRING_PORT];
	std::string port, conf;

	id = id_server;
	service_type = service;
	server_port = server_p;
	broker_port = broker_p;
	server_address = server_addr;
	broker_address = broker_addr;

	/* Allocating ZMQ context */
	try {
		context = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Preparing ZMQ Socket to receive messages on */
	try {
		receiver = new zmq::socket_t(*context, ZMQ_PULL);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	memset(str, '\0', MAX_LENGTH_STRING_PORT);
	sprintf(str, "%d", server_port);
	port.assign(str);
	conf = (COM_PROTOCOL + server_address + ":" + port);
	receiver->connect(conf.c_str());

	std::cout << conf << std::endl;

	/* Preparing ZMQ Socket to send messages */
	try {
		sender = new zmq::socket_t(*context, ZMQ_PUSH);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	memset(str, '\0', MAX_LENGTH_STRING_PORT);
	sprintf(str, "%d", broker_port);
	port.assign(str);
	conf = (COM_PROTOCOL + broker_addr + ":" + port);
	sender->connect(conf.c_str());

	std::cout << conf << std::endl;
}
/**
 * @brief Server denstructor to destroy all the dynamic objects
 * 
 */
Server::~Server()
{
	delete context;
	delete receiver;
	delete sender;
}

void Server::wait_request() 
{	

}

void Server::deliver_service()
{

}
	