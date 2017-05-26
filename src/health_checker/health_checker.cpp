/**
 *
 * health_checker.cpp
 * 
 */
#include "../../include/health_checker.hpp";

#define HB_ARGS 4

int32_t main(int32_t argc, char_t* argv[])
{	
	HealthCheker hc;
	std::string srv_address;("localhost");
	pid_t srv_pid;
	uint16_t srv_port;
	uint8_t hb_liveness;
	int64_t hb_interval;
	
	if (argc < HB_ARGS) {
		std::cerr << "Missing mandatory arguments!" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	srv_pid = *argv[0];
	srv_port = *argv[1];
	hb_liveness = *argv[2];
	hb_interval = *argv[3];

	/* Instanciate the health checker */
	try {
		hb = new HealthCheker(srv_pid, srv_port, hb_liveness, 
			hb_interval);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() <<  std::endl;
		exit(EXIT_FAILURE);
	}
	
	std::cout <<"Server PID: " << (int32_t)srv_pid << std::endl << 
		" Server port: " << (int32_t)srv_port << std::endl <<
		"Liveness: " << (int32_t)hb_liveness << std:: endl <<
		"Interval: " << hb_interval << std::endl;
		
	hb->step();	
	
	delete hb;

	return EXIT_SUCCESS;
}