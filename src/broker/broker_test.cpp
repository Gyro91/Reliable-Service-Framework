/*
 * broker.cpp
 * Main program of the broker
 */

#include "../../include/types.hpp"
#include "../../include/broker_class.hpp"
#include "../../include/test.hpp"


int32_t main(int32_t argc, char_t* argv[])
{	
	RSF_Broker broker(3, ROUTER_PORT_BROKER, REG_PORT_BROKER);

	broker.step();

	return EXIT_SUCCESS;
}