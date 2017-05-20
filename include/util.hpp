/*
 * check_util.hpp
 *
 */

#ifndef INCLUDE_CHECK_UTIL_HPP_
#define INCLUDE_CHECK_UTIL_HPP_

#include <zmq.hpp>
#include <iostream>
#include <vector>
#include "types.hpp"
#include "service.hpp"


extern void get_arg(int32_t, char_t **, uint8_t &, service_type_t &, char_t);

extern zmq::socket_t* add_socket(zmq::context_t *, std::string, uint16_t, 
	int32_t, uint8_t);

extern void send_multi_msg(zmq::socket_t*, std::vector<zmq::message_t>&);

extern void get_arg(int32_t, char_t **, uint8_t &, uint8_t &, char_t);

extern zmq::socket_t* add_socket(zmq::context_t *, std::string, uint16_t, int32_t, uint8_t);

extern void deployment(uint8_t, uint8_t, pid_t *, int32_t *);


#endif /* INCLUDE_CHECK_UTIL_HPP_ */
