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
#include <unordered_map>

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

/* std::unordered_map requires a hash functor in order to do anything.  
 * By default, that hash functor is std::hash<Key> but the standard only 
 * provides specializations for the integral types and pointers.
 * Since we use service_type_t as the key in ServiceDatabase, we provide      * this functor to calculate the hash of each record instance.
 */

struct service_type_hash
{
    std::size_t operator()(const service_type_t& type) const {
        return (std::size_t)type;
    }
};

class ServiceDatabase {

private:
	/* Hash table for registered services */
	std::unordered_map<service_type_t, service_record, service_type_hash> services_db;
	/* This is the index of the current available posistion in the dealer socket list */
	uint16_t next_dealer_skt_index;
public:
	void push_registration(registration_module *reg_mod);
	bool find_registration(service_type_t);
	void print_htable();

	ServiceDatabase();
	~ServiceDatabase();
};

#endif /* INCLUDE_SERVICE_DATABASE_CLASS_HPP_ */