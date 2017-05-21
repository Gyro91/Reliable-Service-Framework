/*
 * broker.cpp
 * Main program of the broker
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <zmq.hpp>
#include "../../include/types.hpp"
#include "../../include/broker_class.hpp"
#include "../../include/communication.hpp"


int32_t main(int32_t argc, char_t* argv[])
{	
	Broker broker(3, ROUTER_PORT_BROKER, REG_PORT_BROKER);

	broker.step();

	return EXIT_SUCCESS;
}