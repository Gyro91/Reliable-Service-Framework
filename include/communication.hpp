/* 
 * communication.hpp
 * In this file some useful macro for the communication are defined
 */

#ifndef INCLUDE_COMMUNICATION_HPP_
#define INCLUDE_COMMUNICATION_HPP_

#include <arpa/inet.h>
#include <string>
#include "service.hpp"

#define TCP_PROTOCOL "tcp://"
#define IPC_PROTOCOL "ipc://"
#define LOCALHOST "localhost"
#define ANY_ADDRESS "*"
#define BIND 0
#define CONNECT 1
#define MAX_LENGTH_STRING_PORT 6
#define EMPTY_MSG (void*)""

#define NUM_FRAMES 3
#define ID_FRAME 0
#define EMPTY_FRAME 1
#define DATA_FRAME 2
#define LENGTH_ID_FRAME 5
#define ENVELOPE 3

#define HEARTBEAT_INTERVAL 2000
#define HC_HEARTBEAT_INTERVAL 5000
#define TIMEOUT_RCV 500
#define REQUEST_TIMEOUT 5000
#define LIVENESS 3
#define WCDPING 500
#define NO_PONG -1

#define MAX_NMR 5

#define PARAM_SIZE 100

/**
 * @brief      client request module for the service
 */
 
struct request_module {
	service_type_t service;
	char_t parameters[PARAM_SIZE];
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

/*
 * @brief      Service module that the broker sends to a server for a service
 */
 
struct service_module {
	/* If true it is a ping, otherwise it's a service 
	 * request */
	bool heartbeat;
	uint32_t seq_id;
	char_t parameters[PARAM_SIZE];
};

/**
 * @class server reply
 * @file communication.hpp
 * @brief data response from a server
 */
 
struct server_reply_t {
	bool heartbeat;
	bool duplicated;
	int32_t result;
	service_type_t service;
	uint8_t id; 
};

/**
 * @brief Function used to serialize the parameters in a string
 * @param str String used to store the serialized parameters
 */

inline void serialize(std::string &str) {}

template<typename head, typename... tail>
void serialize(std::string &str, head h, tail... t)
{
	str.append(std::to_string(h) + " ");
	serialize(str, t...);
}

/**
 * @brief Function used to deserialize the parameters stream
 * @param params Stringstream used to parse the parameters
 */

inline void deserialize(std::stringstream& params) {}

template<typename head, typename... tail>
void deserialize(std::stringstream& params, head& h, tail&... t)
{
	params >> h;
	deserialize(params, t...);
}

#endif /* INCLUDE_COMMUNICATION_HPP_ */