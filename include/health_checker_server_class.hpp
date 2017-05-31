
#ifndef INCLUDE_HEALTH_CHECKER_SERVER_CLASS_HPP_
#define INCLUDE_HEALTH_CHECKER_SERVER_CLASS_HPP_

#include "health_checker_class.hpp"


class  HealthCheckerServer: public HealthChecker {
	
private:
	uint8_t server_id;
	uint8_t service;
	
	
	void restart_process();
public:
	HealthCheckerServer(pid_t pid, uint16_t port, uint8_t server_id, 
		uint8_t service);
	void step();
	~HealthCheckerServer();
};

#endif /* INCLUDE_HEALTH_CHECKER_SERVER_CLASS_HPP_ */

