/**
 * health_checker_class.cpp
 */

#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include "../../include/health_checker_class.hpp"
#include "../../include/util.hpp"
#include "../../include/communication.hpp"

/**
 * @brief Health checker constructor
 * @param srv_pid PID of the monitored server process
 * @param srv_id ID of the monitored service redundant copy
 * @param srv_service Service provided by yhe monitored server
 * @param srv_port Port address for the server socket
 */

HealthChecker::HealthChecker(pid_t pid, uint16_t port)
{
	this->pid = pid;
	this->port = port;
	this->hb_liveness = HEARTBEAT_LIVENESS;
	
	/* Allocating ZMQ context */
	context_init();
}

HealthChecker::~HealthChecker()
{
	delete hb_skt;
	delete ctx;
}

/**
 * @brief Allocates the ZMQ context, adds the needed socket and the poll item.
 */

void HealthChecker::context_init()
{
	std::string address("localhost");
	
	try {
		ctx = new zmq::context_t(1);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	/* Add the dealer socket */
	hb_skt = add_socket(ctx, address, port, ZMQ_REQ, CONNECT);
	
	/* Add the socket to the poll set */
	item = {static_cast<void*>(*hb_skt), 0, ZMQ_POLLIN, 0};
}

/**
 * @brief Checks if the process (broker or server) is already running and if
 * 	  if it's not it creates a new PID file in 'var/tmp/reliab'
 * @param name Name of the PID file to be created
 */

void HealthChecker::check_pid(std::string name)
{
	struct stat sb;
	std::string pid_file(PID_PATH);
	pid_file += name;
	pid_file += ".pid";
	int32_t ret, stat_ret;
	/* Check if the 'reliab' directory already exists */
	stat_ret = stat(PID_PATH, &sb);
	if (stat_ret != 0)
	{
		if (errno == ENOENT) {
			/* The direcotry doesn't exists, create it */
			ret = mkdir(PID_PATH, 0777);
			if (ret != 0) {
				std::cerr << "Cannot create PID directory";
				exit(EXIT_FAILURE);
			}
		} else {
			std::cerr << "Error during PID file creation" <<
				std::endl;
			exit(EXIT_FAILURE);
		}
		/* Create the PID file */
		std::ofstream stream(pid_file);
		stream << pid << std::endl;
		stream.close();
	} else if (stat_ret == 0 && S_ISDIR(sb.st_mode)){
	}
}

