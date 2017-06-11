/*
 * service_database_class.hpp
 *
 */

#ifndef INCLUDE_SERVICE_DATABASE_CLASS_HPP_
#define INCLUDE_SERVICE_DATABASE_CLASS_HPP_
#include <string>
#include <unordered_map>
#include "types.hpp"
#include "util.hpp"
#include "service.hpp"
#include "rsf_api.hpp"

#define SERVICE_NOT_FOUND -1
#define REG_FAIL 0

/**
 * @class request_record_t
 * @brief Instance for a client request
 */
struct request_record_t {
	/* Unique identifier of the client */
	uint32_t client_id;
	/* Container for the responses of the server copies */
	std::vector<int32_t> results;
	/* Service type */
	service_type_t service;
	/* Timeout for the request */ 
	struct timespec timeout;
};

/**
 * @class service_record
 * @file service_database_class.hpp
 * @brief record for the service database
 */
 
struct service_record { 
	/* Owner of the service */
	std::string owner;
	/* Copies of the group that has registered */
	uint8_t num_copies_registered;
	/* Copies of the group that are working correctly */
	uint8_t num_copies_reliable;
	/* Dealer socket port for this service */
	uint16_t dealer_socket;
	/* Index to access in the dealer socket list to the dealer socket 
	 * for this service */
	uint16_t dealer_skt_index;
	/* Seq. number for the ping */
	uint64_t seq_id_ping;
	/* Seq. number for the request */
	uint64_t seq_id_request;
	/* Vector of active requests from the clients */
	std::vector<request_record_t> request_records;
	/* Vectors that points out if in the current timeout it was 
	 * received a pong from a the server copies and the number 
	 * of pong loss */
	std::vector<bool> new_pong;
	std::vector<int8_t> lost_pong;
	
};

/* std::unordered_map requires a hash functor in order to do anything.  
 * By default, that hash functor is std::hash<Key> but the standard only 
 * provides specializations for the integral types and pointers.
 * Since we use service_type_t as the key in ServiceDatabase, we provide      
 * this functor to calculate the hash of each record instance.
 */

struct service_type_hash
{
    std::size_t operator()(const service_type_t& type) const {
        return (std::size_t)type;
    }
};

class ServiceDatabase {

private:
	/* Redundancy for the voter */
	uint8_t nmr;
	/* Hash table for registered services */
	std::unordered_map<service_type_t, service_record, 
	service_type_hash> services_db;
	/* This is the index of the current available posistion 
	 * in the dealer socket list */
	uint16_t next_dealer_skt_index;
public:
	uint16_t push_registration(registration_module *reg_mod, 
		uint16_t &dealer_socket, bool &ready);
	int32_t find_registration(service_type_t);
	int32_t push_result(server_reply_t *server_reply, uint32_t client_id);
	std::vector<int32_t> get_result(service_type_t service, 
		uint32_t client_id);
	void push_request(request_record_t *request_record, 
		service_type_t service);
	void delete_request(service_type_t service, uint32_t client_id);
	void register_pong(uint8_t id_copy, service_type_t service);
	void check_pong(service_type_t service);
	uint8_t get_reliable_copies(service_type_t service);
	uint64_t get_ping_id(service_type_t service);
	std::vector<request_record_t> get_pending_requests(service_type_t 
		service);
	uint64_t get_request_id(service_type_t service);
	void print_htable();

	ServiceDatabase(uint8_t nmr);
	~ServiceDatabase();
};

#endif /* INCLUDE_SERVICE_DATABASE_CLASS_HPP_ */