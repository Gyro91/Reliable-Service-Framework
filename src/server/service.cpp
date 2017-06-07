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
 * @brief Sums the parameters togheter
 * @param x First value to sum
 * @param y Second value to sum
 * @return Sum of the first and the second value
 */

int32_t sum(int32_t x, int32_t y)
{
	return x + y;
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
//		body = &increment;
		body = std::bind(&increment, std::placeholders::_1);
		break;
	case DECREMENT:
//		body = &decrement;
		body = std::bind(&decrement, std::placeholders::_1);
		break;
	case MULTIPLY2:
//		body = &multiply2;
		body = std::bind(&multiply2, std::placeholders::_1);
		break;
	case SUM:
//		body = &sum;
		body = std::bind(&sum, std::placeholders::_1, std::placeholders::_2);
		break;
	default:
		std::cerr << "Service not supported! Server Crash" << std::endl;
		exit(EXIT_FAILURE);
	}

	assert(body != NULL);

	return body;
}