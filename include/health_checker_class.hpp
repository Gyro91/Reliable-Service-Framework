#ifndef INCLUDE_HEALTH_CHECKER_CLASS_HPP_
#define INCLUDE_HEALTH_CHECKER_CLASS_HPP_

#include <zmq.hpp>
#include <string>
#include "types.hpp"

#define HEARTBEAT_LIVENESS 3
#define SRV_OK 0
#define SRV_TIMEOUT 1

#define PID_PATH "/var/tmp/reliab"

class  HealthChecker {
	
protected:
	
	/* ZMQ context */
	zmq::context_t *ctx;
	/* ZMQ socket used to send pings */
	zmq::socket_t *hb_skt;
	zmq::pollitem_t item;
	/* Communication port */
	uint16_t port;
	/* Monitored process pid */
	pid_t pid;
	/* Heartbeat liveness */
	uint8_t hb_liveness;
	
	
	/* Private functions */
	void context_init();
	virtual void restart_process() = 0;
	void check_pid(std::string name);
public:
	HealthChecker(pid_t pid, uint16_t port);
	virtual void step() = 0;
	virtual ~HealthChecker();
};

#endif /* INCLUDE_HEALTH_CHECKER_CLASS_HPP_ */

