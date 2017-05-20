/*
 * service_database_class.hpp
 *
 */

#ifndef INCLUDE_SERVICE_DATABASE_CLASS_HPP_
#define INCLUDE_SERVICE_DATABASE_CLASS_HPP_

#include "types.hpp"
#include "service.hpp"
#include "rs_api.hpp"
#include <string>
#include <map>

struct service_record { 
	/* Owner of the service */
	std::string owner;
	/* Copies of the group that has registered */
	uint8_t num_copies_registered;
	/* Copies of the group that are working correctly */
	uint8_t num_copies_reliable;
	/* Index to access in the dealer socket list to the dealer socket for this service */
	uint16_t dealer_skt_index;
};

class ServiceDatabase {

private:
	/* Table for registered services */
	std::map<service_type_t, service_record> services_db;
	/* This is the index of the current available posistion in the dealer socket list */
	uint16_t next_dealer_skt_index;
public:
	void push_registration(registration_module *reg_mod);
	
	std::map<service_type_t, service_record>::iterator *find_registration(service_type_t);
	ServiceDatabase();
	~ServiceDatabase();
};

#endif /* INCLUDE_SERVICE_DATABASE_CLASS_HPP_ */