#ifndef INCLUDE_HEALTH_CHECKER_BROKER_CLASS_HPP_
#define INCLUDE_HEALTH_CHECKER_BROKER_CLASS_HPP_

#include "health_checker_class.hpp"


class  HealthCheckerBroker: public HealthChecker {
	
	void restart_process();
public:
	HealthCheckerBroker(pid_t pid, uint16_t port);
	void step();
	~HealthCheckerBroker();
};

#endif /* INCLUDE_HEALTH_CHECKER_BROKER_CLASS_HPP_ */