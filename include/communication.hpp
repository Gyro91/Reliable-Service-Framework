/* 
 * communication.hpp
 * In this file some useful macro for the communication are defined
 */

#ifndef INCLUDE_COMMUNICATION_HPP_
#define INCLUDE_COMMUNICATION_HPP_

#define COM_PROTOCOL "tcp://"
#define LOCALHOST "localhost"
#define ANY_ADDRESS "*"
#define BIND 0
#define CONNECT 1
#define REG_PORT_BROKER 5555
#define ROUTER_PORT_BROKER 5559
#define DEALER_START_PORT 6000
#define MAX_LENGTH_STRING_PORT 6

#define ID_FRAME 0
#define EMPTY_FRAME 1
#define DATA_FRAME 2

#define ENVELOPE 3

/**
 * @brief      client request module for the service
 */
struct request_module {
	service_type_t service;
	int32_t parameter;
};

/**
 * @brief      It's the service status
 */
enum service_status_t {
	SERVICE_AVAILABLE, SERVICE_NOT_AVAILABLE, SERVICE_NOT_RELIABLE
};

/**
 * @brief      broker response module for the service
 */
struct response_module {
	service_status_t service_status;
	int32_t result;
};

/**
 * @class server reply
 * @file communication.hpp
 * @brief data response from a server
 */
struct server_reply_t {
	int32_t result;
	service_type_t service;
};

#endif /* INCLUDE_COMMUNICATION_HPP_ */