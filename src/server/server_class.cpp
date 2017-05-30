/**
 *	server_class.cpp
 * 
 */
#include <iostream>
#include <stdio.h>
#include <time.h>
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

Server::Server(uint8_t id, uint8_t service_type, std::string broker_address) 
{	
	this->id = id;
	this->service_type = (service_type_t)service_type;
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
			(service_type_t) service_type, REG_PORT_BROKER, 
			context);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Add the pong socket */
	hc_pong = add_socket(context, ANY_ADDRESS, SERVER_PONG_PORT + id,
		ZMQ_REP, BIND);
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
	int32_t val, val_elab, ret, ping_loss = 0;
	struct timespec tmp_t, time_t;
	bool heartbeat, reg_ok = true;
	
	
	/* Adding the sockets to the poll set */
	zmq::pollitem_t item = {static_cast<void*>(*hc_pong), 0, ZMQ_POLLIN, 0};
	items.push_back(item);;
	
	for (;;) {
		zmq::poll(items, HEARTBEAT_INTERVAL + WCDPING);
		ret = clock_gettime(CLOCK_MONOTONIC, &tmp_t);
		if (ret == -1)
			std::cerr << "Error gettime " << std::endl;
		/* Check for a service request */
		if (!reg_ok && (items[SERVICE_REQUEST_INDEX].revents 
			& ZMQ_POLLIN)) {
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
				ret = clock_gettime(CLOCK_MONOTONIC, &time_t);
				if (ret == -1)
					std::cerr << "Error gettime " << 
					std::endl;
				else
					time_add_ms(&time_t, 
						HEARTBEAT_INTERVAL + WCDPING);
				pong_broker();
			}
		} 
		if (reg_ok) 
			{
			/* Add the reply socket */
			this->broker_port = registrator->registration();
			if (this->broker_port > 0 && this->broker_port <= 65535) 
				{
				/* In this case the REP socket requires 
				 * the connect() method! */
				delete reply;
				reply = add_socket(context, broker_address, 
				broker_port, ZMQ_REP, CONNECT);
				item = {static_cast<void*>(*reply), 0, 
					ZMQ_POLLIN, 0};
				items.push_back(item);
				reg_ok = false;
				ping_loss = 0;
				
			} else if (this->broker_port == 0) {
				std::cerr << "Error in the registration!" << 
				std::endl;
				exit(EXIT_FAILURE);
			} else if (this->broker_port == -1) {
				std::cout << "timeout send expired" << 
				std::endl;
			}
		}
		
		if (items[SERVER_PONG_INDEX].revents & ZMQ_POLLIN) {
			/* Receive the ping from the health checker */
			std::cout << "Received ping from HC" << std::endl;
			pong_health_checker();
		}
 
		if (time_cmp(&tmp_t, &time_t) == 1) {
			/* Timeout expired. It is a Ping loss from the broker */
			if (++ping_loss == LIVENESS) {
				std::cout << "brutte cose" << std::endl;
				items.erase(items.end() - 1);
				reg_ok = true;
		
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

void Server::pong_health_checker()
{
	zmq::message_t msg;
	
	/* Receive the ping */
	hc_pong->recv(&msg);
	msg.rebuild(EMPTY_MSG, 0);
	
	/* Send the pong */
	hc_pong->send(msg);
}