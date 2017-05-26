#include <zmq.hpp>
#include <string>
#include "types.hpp"

#ifndef INCLUDE_HEALTH_CHECKER_CLASS_HPP_
#define INCLUDE_HEALTH_CHECKER_CLASS_HPP_

class  HealthCheker {
	
private:
	
	/* ZMQ context */
	zmq::context_t *ctx;
	/* ZMQ socket used to send pings */
	zmq::socket_t *dealer;
	zmq::pollitem_t item;
	
	/* Server information */
	pid_t srv_pid;
	uint16_t srv_port;
	/* Heartbeat liveness */
	uint8_t hb_liveness;
	/* Heartbeat interval */
	int64_t hb_interval;
	
public:
	HealthCheker(pid_t srv_pid, uint16_t srv_port, uint8_t hb_liveness, 
		int64_t hb_interval);
	void step();
	~HealthCheker();
}

#endif /* INCLUDE_HEALTH_CHECKER_CLASS_HPP_ */

