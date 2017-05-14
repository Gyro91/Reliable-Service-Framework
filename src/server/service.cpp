/*
 * service.cpp
 * In this module, all the service body functions are defined
 */

#include "../../include/service.hpp"
#include <assert.h>
#include <iostream>
#include <stdlib.h> 

/**
 * @brief It increments the value passed to the function
 * @param x Value to be incremented
 * @retval The parameter value incremented 
 */

int32_t increment(int32_t x) 
{
	int32_t ret;

	ret = ++x;

	return ret;
}

/**
 * @brief It decrements the value passed to the function
 * @param x Value to be decremented
 * @retval The parameter value decremented 
 */

int32_t decrement(int32_t x) 
{
	int32_t ret;

	ret = --x;

	return ret;
}

/**
 * @brief It multiply by two the value passed to the function
 * @param x Value to be multiplied
 * @retval The parameter value multiplied by two
 */

int32_t multiply2(int32_t x) 
{
	int32_t ret;

	ret = (x * 2);

	return ret;
}

/**
 * @brief It returns the function pointer to the service
 * @param service It is the service type
 * @retval It is the function pointer to the service
 */

service_body get_service_body(service_type_t service)
{
	service_body body;

	switch (service) {
	case INCREMENT:
		body = &increment;
		break;
	case DECREMENT:
		body = &decrement;
		break;
	case MULTIPLY2:
		body = &multiply2;
		break;
	default:
		std::cerr << "Service not supported! Server Crash" << std::endl;
		exit(EXIT_FAILURE);
	}

	assert(body != NULL);

	return body;
}