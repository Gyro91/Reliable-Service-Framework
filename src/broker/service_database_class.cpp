/*
 *	service_database_class.cpp
 *
 */

#include <iostream>
#include <stdlib.h>
#include "../../include/service_database_class.hpp"

/**
 * @brief ServiceDatabase Constructor
 * 
 */

ServiceDatabase::ServiceDatabase(uint8_t nmr) 
{
	this->nmr = nmr;
	this->next_dealer_skt_index = 0;
}

/**
 * @brief ServiceDatabase Denstructor
 * 
 */

ServiceDatabase::~ServiceDatabase() 
{
	
}

/**
 * @brief      It finds if a service is registered
 *
 * @param[in]  service  The service
 *
 * @return     It returns -1 if there is not a service, otherwise the dealer 
 * index 
 */

int32_t ServiceDatabase::find_registration(service_type_t service)
{	
	std::unordered_map<service_type_t, service_record, 
	service_type_hash>::iterator i = services_db.find(service);
	
	if (i == services_db.end()) {
		std::cout << service << " is not present" << std::endl;	
		return -1;
	} 
		
	return (i->second).dealer_skt_index;
}


/**
 * @brief      Pushes a registration
 *
 * @param      reg_mod  The registration module
 * @param[in]  dealer_socket  The dealer socket
 * 
 * @return     It returns the dealer_socket
 */

uint16_t ServiceDatabase::push_registration(registration_module *reg_mod, 
	uint16_t &dealer_socket, bool &ready)
{
	const service_type_t service_type = reg_mod->service;
	std::unordered_map<service_type_t, service_record, 
		service_type_hash>::iterator i = services_db.find(service_type);

	ready = false;

	if (i == services_db.end()) {
		/* If not present */
		service_record record;
		record.owner = std::string(reg_mod->signature);
		record.num_copies_registered = 1;
		record.num_copies_reliable = 1;
		record.dealer_skt_index = next_dealer_skt_index;
		record.dealer_socket = dealer_socket;
		next_dealer_skt_index++;
		services_db[service_type] = record;

		return dealer_socket++;
		/* Obviously not equal to nmr */
		return REG_OK;
	} else {
		(i->second).num_copies_registered++;
		(i->second).num_copies_reliable++;

		if ((i->second).num_copies_registered == nmr)
			ready = true;

		return ((i->second).dealer_socket);
	}

}

/**
 * @brief It inserts a request from the client in the database
 * @param request record to be inserted 
 */
 
void ServiceDatabase::push_request(request_record_t *request_record, 
	service_type_t service)
{
	std::unordered_map<service_type_t, service_record, 
		service_type_hash>::iterator i = 
		services_db.find(service);
	
	if (i == services_db.end()) {
		std::cerr << "Service not found" << std::endl;
		exit(-1);
	}
	
	(i->second).request_records.push_back(*request_record);
}
/**
 * @brief      It deletes a service request from the db
 *
 * @param[in]  service    The service
 * @param[in]  client_id  The client identifier
 */

void ServiceDatabase::delete_request(service_type_t service, 
	uint32_t client_id)
{
	std::unordered_map<service_type_t, service_record, 
		service_type_hash>::iterator i = 
		services_db.find(service);
	
	if (i == services_db.end()) {
		std::cerr << "Service not found" << std::endl;
		exit(-1);
	}
	
	service_record *record = &i->second;
	
	for (uint32_t j = 0; j < record->request_records.size(); j++) 
		if (record->request_records[j].client_id == client_id) 
			record->request_records.erase(record->
			request_records.begin() + j); 
}

/**
 * @brief It updates with a result from a copy in the server
 * @param server_reply Message from the server
 * @return the number of results received
 */
 
int32_t ServiceDatabase::push_result(server_reply_t *server_reply, 
	uint32_t client_id)
{
	int32_t ret = -1;
	std::unordered_map<service_type_t, service_record, 
		service_type_hash>::iterator i = 
		services_db.find(server_reply->service);

	if (i == services_db.end()) {
		std::cerr << "Service not found" << std::endl;
		exit(-1);
	}
	
	service_record *record = &i->second;
	
	for (uint32_t j = 0; j < record->request_records.size(); j++) {
		if (record->request_records[j].client_id == client_id) {
			record->request_records[j].results.push_back
			(server_reply->result);
			ret = record->request_records[j].results.size();  
		}
	}
	
	return ret;
}

/**
 * @brief It gets the results from the database
 * @param service service type
 * @param client_id Id of the client
 * @return It returns the pointer to results
 */

std::vector<int32_t> ServiceDatabase::get_result(service_type_t service, 
	uint32_t client_id)
{
	std::vector<int32_t> ret;
	std::unordered_map<service_type_t, service_record, 
		service_type_hash>::iterator i = 
		services_db.find(service);

	if (i == services_db.end()) {
		std::cerr << "Service not found" << std::endl;
		exit(-1);
	}
	
	service_record *record = &i->second;
	
	for (uint32_t j = 0; j < record->request_records.size(); j++) {
		if (record->request_records[j].client_id == client_id) {
			ret = record->request_records[j].results; 
		}
	}
	
	return ret;
}

/**
 * @brief      It prints all the pair (key, value) in the database
 */

void ServiceDatabase::print_htable()
{
	for (auto it : services_db) {
    		std::cout << "Service: " << it.first << " Owner: " 
    		<< it.second.owner << " Copies: " << 
    		(uint32_t)it.second.num_copies_registered << 
    		" Socket: " << it.second.dealer_socket <<  std::endl;
		
		for (auto it_v : it.second.request_records) {
			std::cout << "Client id " << (uint32_t)it_v.client_id 
			<< " Voter values ";
			for (auto it_val : it_v.results) 
				std::cout << it_val << " ";
			std::cout << std::endl;
		}
	}
}

