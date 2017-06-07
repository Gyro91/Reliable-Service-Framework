/* 	CLIENT-BROKER
 * 	uint8_t send_request(uint8_t id, int32_t param);
 *	uint8_t get_result(uint32_t* res);
 *
 */
 
/*
 * 	BROKER-SERVER
 *	uint8_t service_request(uint8_t id, int32_t param); 
 */


#ifndef INCLUDE_RS_API_HPP_
#define INCLUDE_RS_API_HPP_

#include <zmq.hpp>
#include <string>
#include <iostream>
#include "types.hpp"
#include "service.hpp"
#include "communication.hpp"
#include <tuple>

#define MAX_LENGTH_SIGNATURE 32

/**
 * @brief      registration module for the server
 */
 
struct registration_module {
	char_t signature[MAX_LENGTH_SIGNATURE];
	service_type_t service;
};

extern int32_t register_service(registration_module *, zmq::socket_t *);
extern bool request_service(request_module &, zmq::socket_t *, int32_t &);

template<typename... Types>
bool request_service2(service_type_t service, zmq::socket_t *socket,
	int32_t& result, Types... args)
{
	response_module response;
	std::string serialized;
	request_module rm;
	
	serialize(serialized, args...);
	std::cout << serialized << std::endl;

	std::strcpy (rm.parameters, serialized.c_str());
	rm.service = service;

	/* Service Request */
	std::cout << "Sizeof rm " << sizeof(rm) << " Sizeof type " <<
		sizeof(request_module) << std::endl;
	zmq::message_t request(sizeof(rm));
       	memcpy(request.data(), (void *) &rm, sizeof(rm));
        std::cout << "Request for service "<< rm.service << std::endl;
        socket->send(request);
        std::cout << "Sended request"<< std::endl;
        
        /* Get response module */
        zmq::message_t reply;
    	socket->recv(&reply);
        response = *(static_cast<response_module*> (reply.data()));
        std::cout << "Status " << response.service_status << std::endl;
        if (response.service_status == SERVICE_NOT_RELIABLE) {
		std::cout << "Service not reliable" << std::endl;
		return false;
	} else if (response.service_status ==  SERVICE_NOT_AVAILABLE) {
		std::cout << "Service not available" << std::endl;
        	return false;
	}
        result = response.result;

        return true;
}

#endif


