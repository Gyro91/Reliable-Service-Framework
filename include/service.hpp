/*
 * service.hpp
 *
 */

#ifndef INCLUDE_SERVICE_HPP_
#define INCLUDE_SERVICE_HPP_

#include "types.hpp"
typedef double(*service_body)(int32_t);
extern void get_service_body(uint8_t service);

#endif /* INCLUDE_SERVICE_HPP_ */