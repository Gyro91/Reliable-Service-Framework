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
#include "types.hpp"
#include "service.hpp"
#include "communication.hpp"

#define MAX_LENGTH_SIGNATURE 32

/**
 * @brief      registration module for the server
 */
struct registration_module {
	char_t signature[MAX_LENGTH_SIGNATURE];
	service_type_t service;
};

extern uint16_t register_service(registration_module *, zmq::socket_t *);
extern bool request_service(request_module &, zmq::socket_t *, int32_t &);

#endif


