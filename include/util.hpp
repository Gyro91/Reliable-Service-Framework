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

#define CONSOLE &std::cout
#define ABS_YEAR 1900


extern void get_arg(int32_t, char_t **, uint8_t &, uint8_t &, char_t);
extern void get_arg(int32_t, char_t **, service_type_t &, char_t);

extern zmq::socket_t* add_socket(zmq::context_t *, std::string, uint16_t, 
	int32_t, uint8_t);

extern void send_multi_msg(zmq::socket_t*, std::vector<zmq::message_t>&);

extern zmq::socket_t* add_socket(zmq::context_t *, std::string, uint16_t, 
	int32_t, uint8_t);

extern void deployment(uint8_t, uint8_t, pid_t *, int32_t *);

extern void time_copy(struct timespec *, const struct timespec *);

extern void time_add_ms(struct timespec *, int64_t);

extern void time_add_ns(struct timespec *, int64_t);

extern int32_t time_cmp(struct timespec *, struct timespec *t2);

extern void busy_wait(uint32_t );

extern void write_log(std::ostream *, std::string, std::string);

#endif /* INCLUDE_CHECK_UTIL_HPP_ */
