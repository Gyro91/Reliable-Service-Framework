/*
 * service.hpp
 *
 */

#ifndef INCLUDE_SERVICE_HPP_
#define INCLUDE_SERVICE_HPP_

#include "types.hpp"

enum service_type_t {
	INCREMENT, DECREMENT, MULTIPLY2
};

typedef int32_t (*service_body)(int32_t);

extern service_body get_service_body(service_type_t service);
extern int32_t increment(int32_t x);
extern int32_t decrement(int32_t x);
extern int32_t multiply2(int32_t x);

#endif /* INCLUDE_SERVICE_HPP_ */