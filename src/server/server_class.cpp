/**
 *	server_class.cpp
 * 
 */
#include <iostream>
#include <stdio.h>
#include "../../include/server_class.hpp"
#include "../../include/communication.hpp"
#include "../../include/util.hpp"

#define MAX_LENGTH_STRING_PORT 6 /* Max number of char needed for data port */

/**
 * @brief Server constructor that initializes alle the private data and
 * 	  claims memory for ZPQ sockets. Then it connects to the socket.
 * @param id_server Identifier among server copies
 * @param service_t Service type to be deployed
 * @param server_p Server receive port
 * @param broker_addr Broker address
 * @param broker_p Broker port
 * 
 */

Server::Server(uint8_t id, uint8_t service_type, std::string broker_address, 
		uint16_t broker_port) 
{	
	this->id = id;
	this->service_type = (service_type_t)service_type;
	this->broker_port = broker_port;
	this->broker_address = broker_address;

	service = get_service_body(this->service_type);

	/* Allocating ZMQ context */
	try {
		context = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}	

	try {
		registrator = new Registrator(broker_address, 
			(service_type_t) service_type, REG_PORT_BROKER, context);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}	
}

/**
 * @brief Server denstructor to destroy all the dynamic objects
 * 
 */

Server::~Server()
{
	delete context;
	delete reply;
	delete registrator;
}

/**
 * @brief Server step function used to perform all the computation.
 */

void Server::step()
{	 
	int32_t val, val_elab;
	bool heartbeat;
	
	this->broker_port = registrator->registration();
	if (this->broker_port > 0 && this->broker_port <= 65535) {
		/* In this case the REP socket requires the connect() method! */
		reply = add_socket(context, broker_address, broker_port, 
			ZMQ_REP, CONNECT);
	} else {
		std::cerr << "Error in the registration!" << std::endl;
		exit(EXIT_FAILURE);
	}
	/* Adding the port to the poll set */
	zmq::pollitem_t item = {static_cast<void*>(*reply), 0, ZMQ_POLLIN, 0};
	items.push_back(item);
	
	for (;;) {
		
		zmq::poll(items, HEARTBEAT_INTERVAL);
		
		/* Check for a service request */
		if (items[SERVICE_REQUEST_INDEX].revents & ZMQ_POLLIN) { 
			/* Receiving the value to elaborate */
			heartbeat = receive_request(&val);
			if (!heartbeat) {
				/* Elaborating */
				val_elab = service(val);
				sleep(1);
				/* Sending back the result */
				deliver_service(val_elab);
			} else {
				std::cout << "Server " << (int32_t) id << 
				": Ping from Broker" << std::endl;
				pong_broker();
			}
		}
	}
}

/**
 * @brief Receive the request message from the broker and returns 
 * 	  the data contained inside it.
 * @param val	Reference to return the message data.
 * 
 * @return it returns true if it is a broker ping
 */

bool Server::receive_request(int32_t* val)
{
	zmq::message_t msg;
	service_module sm;
	
	reply->recv(&msg);
	sm = *(static_cast<service_module *> (msg.data()));
	
	if (sm.heartbeat == false) {
		*val = sm.parameter;
		std::cout << "Server " << (int32_t)id << " received: " <<
		*val << std::endl;
	}
	
	return sm.heartbeat;
}

/**
 * @brief Sends back to the broker the results of the service.
 * @param val	Service result to be sent.
 */

void Server::deliver_service(int32_t val)
{
	server_reply_t server_reply;
	zmq::message_t msg(sizeof(server_reply_t));
	
	server_reply.id = NO_PONG;
	server_reply.result = val;
	server_reply.service = service_type;
	
	memcpy(msg.data(), (void *) &server_reply, sizeof(server_reply_t));
	reply->send(msg);
	std::cout << "Server " << (int32_t)id << " sent: " << val <<
		std::endl;
}

/**
 * @brief It sends a pong to the broker 
 */
 
void Server::pong_broker()
{
	server_reply_t server_reply;
	zmq::message_t msg(sizeof(server_reply_t));
	
	server_reply.id = id; /* Pong from server id */
	server_reply.service = service_type;
	
	memcpy(msg.data(), (void *) &server_reply, sizeof(server_reply_t));
	reply->send(msg);
}