#include <zmq.hpp>
#include <string>
#include "types.hpp"

#ifndef INCLUDE_HEALTH_CHECKER_CLASS_HPP_
#define INCLUDE_HEALTH_CHECKER_CLASS_HPP_

#define HEARTBEAT_INTERVALL 1000
#define HEARTBEAT_LIVENESS 3
#define SRV_OK 0
#define SRV_TIMEOUT 1

class  HealthCheker {
	
private:
	
	/* ZMQ context */
	zmq::context_t *ctx;
	/* ZMQ socket used to send pings */
	zmq::socket_t *hb_skt;
	zmq::pollitem_t item;
	
	/* Server information */
	pid_t srv_pid;
	uint16_t srv_port;
	uint8_t srv_expiry;
	/* Heartbeat liveness */
	uint8_t hb_liveness;
	
public:
	HealthCheker(pid_t srv_pid, uint16_t srv_port);
	void step();
	~HealthCheker();
};

#endif /* INCLUDE_HEALTH_CHECKER_CLASS_HPP_ */

