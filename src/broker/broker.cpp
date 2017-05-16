/*
 * broker.cpp
 * Main program of the broker
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../../include/types.hpp"
#include "../../include/broker_class.hpp"
#include <zmq.hpp>


int32_t main(int32_t argc, char_t* argv[])
{	
	Broker broker(3, 5559, 5555);

	broker.step();

	return EXIT_SUCCESS;
}