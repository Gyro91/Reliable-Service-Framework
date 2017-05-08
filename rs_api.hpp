/* CLIENT-BROKER
 * 
 */
 
uint8_t send_request(uint8_t id, int32_t param);
uint8_t get_result(uint32_t* res);

/* BROKER-SERVER
 * 
 */

uint8_t service_request(uint8_t id, int32_t param)
