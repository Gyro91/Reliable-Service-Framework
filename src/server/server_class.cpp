#include "../../include/server_class.hpp"
#include <iostream>


Server::Server(uint8_t id_server, std::string server_addr, 
		uint16_t server_p, std::string broker_addr, 
		uint16_t broker_p) 
{
	id = id_server;
	server_port = server_p;
	broker_port = broker_p;

	server_address = server_addr;
	broker_address = broker_addr;
}

Server::~Server()
{
	

}


void Server::wait_request() 
{	

}

void Server::deliver_service()
{

}
	