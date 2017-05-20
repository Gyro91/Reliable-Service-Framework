/*
 * service_database_class.hpp
 *
 */

#ifndef INCLUDE_SERVICE_DATABASE_CLASS_HPP_
#define INCLUDE_SERVICE_DATABASE_CLASS_HPP_

#include "types.hpp"
#include "service.hpp"

struct service_record {
	/* Unique identifier of the server copies group */
	uint32_t id; 
	/* Service type of the server copies group */
	service_type_t service_type;
	/* Copies of the group that has registered */
	uint8_t num_copies_registered;
	/* Copies of the group that are working correctly */
	uint8_t num_copies_reliable;
	/* Index to access in the dealer socket list to the dealer socket for this service */
	uint16_t dealer_skt_index;
};

class ServiceDatabase {

private:
	std::list<service_record> services_list;
	/* This is the index of the current available posistion in the dealer socket list */
	uint16_t next_dealer_skt_index;
public:
	int32_t push_registration(registration_module *);

	ServiceDatabase();
	~ServiceDatabase();
};

#endif /* INCLUDE_SERVICE_DATABASE_CLASS_HPP_ */