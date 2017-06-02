/**
 *	server_class.cpp
 * 
 */
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
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
	this->service = get_service_body(this->service_type);
	this->service_thread.service = service;
	this->service_thread.service_type = this->service_type;
	this->ping_id = 0;
	
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
	uint64_t received_id;
	int32_t val, ping_loss = 0;
	struct timespec tmp_t, time_t;
	bool heartbeat, reg_ok = false;
	
	
	/* Adding the sockets to the poll set */
	zmq::pollitem_t item = {static_cast<void*>(*hc_pong), 0, ZMQ_POLLIN, 0};
	items.push_back(item);;
	
	for (;;) {
		zmq::poll(items, HEARTBEAT_INTERVAL);
		clock_gettime(CLOCK_MONOTONIC, &tmp_t);
		
		/* Check for a service request */
		if (reg_ok && (items[SERVICE_REQUEST_INDEX].revents 
			& ZMQ_POLLIN)) {
			std::cout << "Receiving Server " << (int32_t) id << std::endl;
			/* Receiving the value to elaborate */
			heartbeat = receive_request(&val, &received_id);
			std::cout << "Received Server " << (int32_t) id << std::endl;
			clock_gettime(CLOCK_MONOTONIC, &time_t);
			time_add_ms(&time_t, 
					HEARTBEAT_INTERVAL + WCDPING);
			std::cout << "Message " << received_id << " Server " <<
			(int32_t) id << " expected " << ping_id << std::endl;
			if (ping_id == 0)
				ping_id = received_id;
			if (!heartbeat) {
				create_thread(val);
//				/* Elaborating */
//				val_elab = service(val);
//				//busy_wait(1000);
//				/* Sending back the result */
//				deliver_service(val_elab);
			} else if (received_id == ping_id) {
				std::cout << "Server " << (int32_t) id << 
				": Ping " << ping_id << "from Broker" 
				<< std::endl;
				ping_id++;
				pong_broker();
			} 
		}
		
		if (items[SERVER_PONG_INDEX].revents & ZMQ_POLLIN) {
			/* Receive the ping from the health checker */
			std::cout << "Server " << (int32_t) id 
			<< " Received ping from HC" << std::endl;
			pong_health_checker();
		}
		
		if (!reg_ok) 
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
				reg_ok = true;
				ping_loss = 0;
				clock_gettime(CLOCK_MONOTONIC, &time_t);
				time_add_ms(&time_t, 
					HEARTBEAT_INTERVAL + WCDPING);

			} else if (this->broker_port == 0) {
				std::cerr << "Error in the registration!" << 
				std::endl;
				exit(EXIT_FAILURE);
			} else if (this->broker_port == -1) {
				std::cout << "timeout receive expired" << 
				std::endl;
			}
		}
 
		if (time_cmp(&tmp_t, &time_t) == 1 && reg_ok) {
			/* Timeout expired. It is a Ping loss from the broker */
			if (++ping_loss == LIVENESS) {
				std::cout << "Server " << (int32_t)id 
				<< "brutte cose" << std::endl;
				items.erase(items.end() - 1);
				reg_ok = false;
		
			}
		}
	
	}
}

/**
 * @brief Receive the request message from the broker and returns 
 * 	  the data contained inside it.
 * @param val	Reference to return the message data.
 * @param received_id Where to put the seq id of the received message
 * 
 * @return it returns true if it is a broker ping
 */

bool Server::receive_request(int32_t* val, uint64_t* received_id)
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
	
	*received_id = sm.seq_id;
	
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

void *task(void *arg) 
{	
	int32_t val_elab;
	server_reply_t server_reply;
	zmq::message_t msg(sizeof(server_reply_t));
	service_thread_t *st = static_cast<service_thread_t *> (arg);
	
	val_elab = st->service(st->parameter);
	
	server_reply.id = NO_PONG;
	server_reply.result = val_elab;
	server_reply.service = st->service_type;
	
	memcpy(msg.data(), (void *) &server_reply, sizeof(server_reply_t));
	st->skt->send(msg);
	std::cout << "Thread " << (int32_t)st->id << " sent: " << val_elab <<
		std::endl;
	
	pthread_exit(NULL);
}

void Server::create_thread(int32_t parameter)
{
	pthread_t tid;
	int32_t ret;
	
	service_thread.skt = reply;
	service_thread.parameter = parameter;
	
	ret = pthread_create(&tid, NULL, task, 
		(void *) &service_thread);
	if (ret != 0) {
		perror("Errror pthread_create");
		exit(EXIT_FAILURE);
	}
	
}