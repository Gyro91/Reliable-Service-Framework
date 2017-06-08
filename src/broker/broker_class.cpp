/*
 *	broker_class.cpp
 *
 */
#include <string>
#include <iostream>
#include <unistd.h>
#include "../../include/broker_class.hpp"
#include "../../include/communication.hpp"
#include "../../include/util.hpp"
#include "../../include/rs_api.hpp"

/**
 * @brief Broker constructor that initializes alle the private data and
 * 	  claims memory for ZPQ sockets. Then it connects to the socket.
 * @param nmr Redundancy for the voter
 * @param port_router It is the port for client communication
 * @param port_reg It is the port for the server registration
 * 
 */

Broker::Broker(uint8_t nmr, uint16_t port_router, uint16_t port_reg) 
{	
	int32_t opt;

	this->nmr = nmr;
	this->port_router = port_router;
	this->port_reg = port_reg;
	this->available_dealer_port = DEALER_START_PORT;

	/* Allocating ZMQ context */
	try {
		context = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Router socket creation */
	router = add_socket(context, ANY_ADDRESS, port_router, ZMQ_ROUTER, 
		BIND);
	/* This option is used to enable error messages when an invalid
	 * identity is used to send a message with a ROUTER socket
	 */
	opt = 1;
	router->setsockopt(ZMQ_ROUTER_MANDATORY, &opt, sizeof(int32_t));
	
	/* Registration socket creation */
	reg = add_socket(context, ANY_ADDRESS, port_reg, ZMQ_ROUTER, BIND);
	/* Health checker socket creation */
	hc = add_socket(context, ANY_ADDRESS, BROKER_PONG_PORT, ZMQ_REP, BIND);
	/* Initialize the poll set */
	zmq::pollitem_t tmp = {static_cast<void*>(*router), 0, ZMQ_POLLIN, 0};
	items.push_back(tmp);
	tmp = {static_cast<void*>(*reg), 0, ZMQ_POLLIN, 0};
	items.push_back(tmp);
	tmp = {static_cast<void*>(*hc), 0, ZMQ_POLLIN, 0};
	items.push_back(tmp);

	/* Creating a Service Database*/
	db = new ServiceDatabase(nmr);

	my_name = "Broker";
}



/**
 * @brief      Destroys the object.
 */

Broker::~Broker()
{
	delete router;
	delete reg;
	delete db;
	delete context;
}

/**
 * @brief      step function
 */

void Broker::step()
{
	for (;;) {
		zmq::poll(items, 50);
		clock_gettime(CLOCK_MONOTONIC, &now);
		
		/* Check the ping from the health checker*/
		if (items[HC_POLL_INDEX].revents & ZMQ_POLLIN) {
			write_log(my_name, "Received ping from HC");
			pong_health_checker();	
		}
		/* Check for a registration request */
		if (items[REG_POLL_INDEX].revents & ZMQ_POLLIN) 
			get_registration();			
	
		/* Check for messages on the ROUTER socket */
		if (items[ROUTER_POLL_INDEX].revents & ZMQ_POLLIN) 
			get_request();		
		
		/* Check for messages on the DEALER sockets */
		for (uint32_t i = 0; i < dealer.size(); i++) 
			if (items[i + DEALER_POLL_INDEX].revents & 
				ZMQ_POLLIN) 
				get_response(i);	
		
		for (uint32_t i = 0; i < timeout.size(); i++) {
			if (time_cmp(&now, &timeout[i]) == 1) {
				write_log(my_name, "Heartbeat Timeout expired");
				db->check_pong(available_services[i]);
				ping_server(i, available_services[i]);
				update_timeout(available_services[i]);
			}
		}
		check_pending_requests();
	}
}

/**
 * @brief      Adds a dealer.
 *
 * @param[in]  dealer_port  The dealer port
 */

void Broker::add_dealer(uint16_t dealer_port)
{	
	zmq::pollitem_t item;

	dealer.push_back(add_socket(context, ANY_ADDRESS, dealer_port, 
		ZMQ_DEALER, BIND));
	
	item = {static_cast<void*>(*dealer.back()), 0, ZMQ_POLLIN, 0};
	
	items.push_back(item);
}


/**
 * @brief      Gets the request from a client 
 */
 
void Broker::get_request()
{	
	int32_t ret;
	request_module request;
	response_module response;
	request_record_t request_record;
	uint8_t num_copies_reliable;
	service_module sm;
	std::vector<zmq::message_t> buffer_in(NUM_FRAMES);

	/* Receive multiple messages,
	 * one frame at a time */
	for (uint8_t i = 0; i < ENVELOPE; i++) {
		router->recv(&buffer_in[i]);
		if (i == ID_FRAME) {
			/* We get the client id and we add a request record */
			uint8_t *p = (uint8_t *) buffer_in[i].data();
			p++;
			request_record.client_id = *((uint32_t *) p);
		}
	}

	request = *(static_cast<request_module*> (buffer_in[DATA_FRAME].data())); 
	ret = db->find_registration(request.service);
	if (ret == -1) {
		/* Service not available */
		response.service_status = SERVICE_NOT_AVAILABLE;
		buffer_in[DATA_FRAME].rebuild((void*) &response,
			sizeof(response_module));
		send_multi_msg(router, buffer_in);

	} else {
		/* Service available */
		sm.heartbeat = false;
		sm.seq_id = db->get_request_id(request.service);
		memcpy(&sm.parameters, request.parameters,
			sizeof(request.parameters));
		buffer_in[DATA_FRAME].rebuild((void*) &sm,
			sizeof(service_module));
		num_copies_reliable = db->get_reliable_copies(request.service);
		/* Forwarding the parameter */
		for (uint8_t j = 0; j < num_copies_reliable; j++)
			send_multi_msg(dealer[ret], buffer_in);
		/* Saving the request in the db */
		db->push_request(&request_record, request.service);
		/* Postponing timeout */
		update_timeout(request.service);
	}
}

/**
 * @brief      Gets the registration from a server.
 */

void Broker::get_registration()
{	
	int32_t more;
	size_t more_size;
	bool already_registered = false, ready = false;
	zmq::message_t message;

	for (uint8_t i = 0; i < ENVELOPE; i++) {
		reg->recv(&message);
		more_size = sizeof(more);
		reg->getsockopt(ZMQ_RCVMORE, &more, 
			&more_size);

		if (more == 1) 
			reg->send(message, ZMQ_SNDMORE);
		if (!more) {
			/* Receiving the registration module */
			registration_module rm = 
			*(static_cast<registration_module*> 
				(message.data()));
			
			/* Registering */
			uint16_t ret = 
			db->push_registration(&rm, available_dealer_port, 
				ready);
			
			for (uint32_t i = 0; i < available_services.size(); i++)
				if (available_services[i] == rm.service)
					already_registered = true;
			/* If all the copies are registered */
			if (ready && !already_registered) {
				/* Make the service available */
				available_services.push_back(rm.service);
				/* Add a dealer port */
				add_dealer(ret);
				/* Setting the timeout for the copies */
				struct timespec timeout_tmp;
				clock_gettime(CLOCK_MONOTONIC, &timeout_tmp);
				time_add_ms(&timeout_tmp, HEARTBEAT_INTERVAL);
				timeout.push_back(timeout_tmp);
			}
			db->print_htable();
			/* Sending back the dealer port */
			zmq::message_t reply(sizeof(ret));
			memcpy(reply.data(), 
				(void *) &ret, sizeof(ret));
			reg->send(reply, 0);
		}
	}
	
}

/**
 * @brief      Gets the response from a server
 *
 * @param[in]  dealer_index  The dealer index
 * @param      num_replies   the replies number for the service
 * @param      dealer_in     The dealer buffer
 */

void Broker::get_response(uint32_t dealer_index)
{	
	int32_t num_copies, ret, result;
	uint32_t i = 0;
	server_reply_t server_reply;
	response_module response;
	zmq::message_t message;
	uint32_t client_id;
	std::vector<zmq::message_t> buffer_in(NUM_FRAMES);

	/* Receiving all the messages */

	for (i = 0; i < ENVELOPE; i++) {
		dealer[dealer_index]->recv(&buffer_in[i], ZMQ_DONTWAIT);
		if (i == ID_FRAME) {
			uint8_t *p = (uint8_t *) buffer_in[i].data();
			p++;
			client_id = *((uint32_t *) p);
		}			
	}
	
	server_reply = *(static_cast<server_reply_t*>
			(buffer_in[DATA_FRAME].data()));
			
	if (server_reply.heartbeat) {
		write_log(my_name, "Pong from Service " + 
			std::to_string(server_reply.service) + " Server" +
			std::to_string((int32_t) server_reply.id));
		db->register_pong(server_reply.id, server_reply.service);
	} else {
		if (!server_reply.duplicated) {
			num_copies = db->push_result(&server_reply, client_id);
			if (num_copies > (nmr / 2)) {
				ret = vote(db->get_result(server_reply.service, 
					client_id), result);
				if (ret >= 0) {
					/* Replace the data frame with the
					 * one obtained from the voter.
					 */
					response.service_status = 
						SERVICE_AVAILABLE;
					response.result = result;
					buffer_in[DATA_FRAME].rebuild((void*)
						&response, 
						sizeof(response_module));
					send_multi_msg(router, buffer_in);
					/* Deleting service request */
					db->delete_request(server_reply.service, 
						client_id);
				} else if (num_copies == nmr) {
					response.service_status = 
						SERVICE_NOT_RELIABLE;
					response.result = result;
					buffer_in[DATA_FRAME].rebuild((void*)
						&response, 
						sizeof(response_module));
					send_multi_msg(router, buffer_in);
					/* Deleting service request */
					db->delete_request(server_reply.service,
						client_id);
					/*Sending not reliable service*/
				}
			}
		}
	}
}

/**
 * @brief Implements the voting logic
 * @param values List containing the values returned from the servers
 * @return >0 index of the majority value, -1 there is no majority
 */

int8_t Broker::vote(std::vector<int32_t> values, int32_t &result)
{	
	std::vector<uint8_t> count;
	uint8_t max = 0;
	
	for (uint8_t i = 0; i < values.size(); i++) {
		count.push_back(0);
		for (uint8_t j = 0; j < values.size(); j++) {
			if (values[i] == values[j])
				count[i]++;
		}
	}
	
	for (uint8_t i = 0; i < count.size(); i++)
			if (count[i] > count[max])
				max = i;
				
	if (count[max] > (nmr / 2)) {
		result = values[max];
		return max;
	} else return -1;

}

/**
 * @brief It sends a ping to all the servers
 */
 
void Broker::ping_servers()
{
	service_module sm;
	uint8_t num_copies_reliable;
	std::vector<zmq::message_t> buffer_in(NUM_FRAMES);
	char_t address_ping[LENGTH_ID_FRAME];
	
	address_ping[0] = 0;
	memset((address_ping + 1), 'a', LENGTH_ID_FRAME - 1);
	
	/* In order to reuse the same dealer port for receiving pong and
	 * results we have to emulate the router-dealer-rep pattern */
	sm.heartbeat = true;
	for (uint32_t i = 0; i < dealer.size(); i++) {
		buffer_in[ID_FRAME].rebuild((void*) &address_ping[0], 
			sizeof(address_ping));
		buffer_in[EMPTY_FRAME].rebuild((void*) "", 0);
		buffer_in[DATA_FRAME].rebuild((void*) &sm,
			sizeof(service_module));
		num_copies_reliable = 
			db->get_reliable_copies(available_services[i]);
		
		for(uint8_t j = 0; j < num_copies_reliable; j++) 
			send_multi_msg(dealer[i], buffer_in);
	}
}

/**
 * @brief It sends a ping to a specific group of servers
 * @param i index to the servers group
 * @param service service type
 */

void Broker::ping_server(uint8_t i, service_type_t service)
{
	service_module sm;
	uint8_t num_copies_reliable;
	std::vector<zmq::message_t> buffer_in(NUM_FRAMES);
	char_t address_ping[LENGTH_ID_FRAME];
	
	address_ping[0] = 0;
	memset((address_ping + 1), 'a', LENGTH_ID_FRAME - 1);
	
	/* In order to reuse the same dealer port for receiving pong and
	 * results we have to emulate the router-dealer-rep pattern */
	sm.heartbeat = true;
	sm.seq_id = db->get_ping_id(service);
	buffer_in[ID_FRAME].rebuild((void*) &address_ping[0], 
		sizeof(address_ping));
	buffer_in[EMPTY_FRAME].rebuild((void*) "", 0);
	buffer_in[DATA_FRAME].rebuild((void*) &sm, sizeof(service_module));
	num_copies_reliable = db->get_reliable_copies(available_services[i]);
	
	for(uint8_t j = 0; j < num_copies_reliable; j++) {
		send_multi_msg(dealer[i], buffer_in);
		write_log(my_name, "Sending ping " + 
			std::to_string(sm.seq_id) + " to Service " + 
			std::to_string(service) + " Server " + 
			std::to_string((int32_t) j));
	}
}

/**
 * @brief It prints all the available services 
 */
 
void Broker::print_available_services()
{	
	for (uint32_t i = 0; i < available_services.size(); i++)
		write_log(my_name, "Service " +
			std::to_string(available_services[i])); 
}

/**
 * @brief Sends the pong message to the health checker
 */

void Broker::pong_health_checker()
{
	zmq::message_t msg;
	
	/* Receive the ping */
	hc->recv(&msg);
	msg.rebuild(EMPTY_MSG, 0);
	
	/* Send the pong */
	hc->send(msg);
}

/**
 * @brief Updates the timeout associated to a service
 * @param service Service
 */

void Broker::update_timeout(service_type_t service)
{
	uint32_t i;
	struct timespec timeout_tmp;
	
	for (i = 0; i < available_services.size(); i++)
		if (available_services[i] == service)
			break;
	
	clock_gettime(CLOCK_MONOTONIC, &timeout_tmp);
	time_add_ms(&timeout_tmp, HEARTBEAT_INTERVAL);
	timeout[i] = timeout_tmp;
}

/**
 * @brief Checks if a service timeout has expired
 */

void Broker::check_pending_requests()
{
	int32_t ret, result;
	struct timespec now;
	std::vector<zmq::message_t> buffer_in(NUM_FRAMES);
	char_t address[LENGTH_ID_FRAME];
	response_module response;
	
	clock_gettime(CLOCK_MONOTONIC, &now);
	for (uint32_t i = 0; i < available_services.size(); i++) {
		std::vector<request_record_t> pending_requests = 
			db->get_pending_requests(available_services[i]);
		for (uint32_t j = 0; j < pending_requests.size(); j++) {
			if (time_cmp(&now, &pending_requests[j].timeout) == 1) {
				ret = vote(pending_requests[j].results, result);
				if (ret >= 0) {
					response.service_status = 
						SERVICE_AVAILABLE;
				} else {
					/* Sending not reliable service */
					response.service_status = 
						SERVICE_NOT_RELIABLE;
				}
				address[0] = 0;
				memcpy((address + 1), &pending_requests[j].
					client_id, LENGTH_ID_FRAME - 1);
				response.result = result;
				buffer_in[ID_FRAME].rebuild((void*) &address[0],
					sizeof(address));
				buffer_in[EMPTY_FRAME].rebuild((void*)"", 0);
				buffer_in[DATA_FRAME].rebuild((void*) &response,
					sizeof(response_module));
				send_multi_msg(router, buffer_in);
				/* Deleting service request */
				db->delete_request((service_type_t)i,
					pending_requests[j].client_id);
			}
		}
	}
}