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

	/* Initialize the poll set */
	zmq::pollitem_t tmp = {static_cast<void*>(*router), 0, ZMQ_POLLIN, 0};
	items.push_back(tmp);
	tmp = {static_cast<void*>(*reg), 0, ZMQ_POLLIN, 0};
	items.push_back(tmp);

	/* Creating a Service Database*/
	db = new ServiceDatabase(nmr);
}



/**
 * @brief      Destroys the object.
 */

Broker::~Broker()
{
	delete context;
	delete router;
	delete reg;
	delete db;
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

void Broker::step()
{
	int32_t more;
	uint8_t num_replies = 0;
	int32_t serv_values[3];
	uint8_t ret;
	std::vector<zmq::message_t> router_in(3);
	std::vector<zmq::message_t> dealer_in(3);

	for (;;) {
		zmq::message_t route_id, empty, data, tmp, message;

		zmq::poll(items, -1);
		std::cout << "Waking up" << std::endl;
		/* Check for messages on the ROUTER socket */
		if (items[0].revents & ZMQ_POLLIN) {
			/* Receive multiple messages,
			 * one frame at a time */
			for (uint8_t i = 0; i < ENVELOPE; i++) {
				router->recv(&tmp);
				router_in[i].copy(&tmp);
			}

			/* Send the data */
			for (uint8_t i = 0; i < 3; i++)
				send_multi_msg(dealer[0], router_in);

		}
		/* Check for a registration request */
		if (items[1].revents & ZMQ_POLLIN) {

			for(uint8_t i = 0; i < ENVELOPE; i++) {
	       			reg->recv(&message);
	       			size_t more_size = sizeof(more);
                		reg->getsockopt(ZMQ_RCVMORE, &more, 
                			&more_size);
				
				if (more == 1) 
					reg->send(message, ZMQ_SNDMORE);
	       	   		if (!more) {
	       	   			bool ready = false;
	       	 			/* Receiving the registration module */
					std::cout << "Receiving registration" 
					<< std::endl;
	       	   			registration_module rm = 
	       	   			*(static_cast<registration_module*> 
	       	   				(message.data()));
	            		
       					/* Registrating */
       					uint16_t ret = 
       					db->push_registration(&rm,
       						available_dealer_port, ready);
       					/* If all the copies are registered */
       					if (ready) 
       						add_dealer(ret);
       					
       					
       					db->print_htable();
	       	   			/* Sending back the result */
       					zmq::message_t reply(sizeof(ret));
       					memcpy(reply.data(), 
       						(void *) &ret, sizeof(ret));
	       	   			reg->send(reply, 0);
	       	   		}
			}
		}
		/* Check for messages on the DEALER socket */
		for (uint32_t i = 0; i < dealer.size(); i++) {
			if (items[i + 2].revents & ZMQ_POLLIN) {
				/* Receiving all the messages */
				for (uint8_t j = 0; j < ENVELOPE; j++) {
					dealer[i]->recv(&tmp);
					dealer_in[j].copy(&tmp);
				}
			
				/* Store the replies from the servers */
				serv_values[num_replies] = 
				(*(static_cast<int32_t*> 
					(dealer_in[DATA_FRAME].data())));
				num_replies++;
			}
			if (num_replies == 3) {
				ret = vote(serv_values);
				if (ret >= 0) {
					/* Replace the data frame with the 
					 * one obtained from the voter.
				 	 */
					dealer_in[DATA_FRAME].rebuild(
						(void*)&serv_values[ret],
						sizeof(serv_values[ret]));
					send_multi_msg(router, dealer_in);
					num_replies = 0;
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

uint8_t Broker::vote(int32_t values[])
{
	if (values[0] == values[1] || values[0] == values [2])
		return 0;
	else if (values[1] == values[2])
		return 1;
	else
		return -1;
}