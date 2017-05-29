/*
 * util.cpp
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <zmq.hpp>
#include <iostream>
#include <sys/wait.h>
#include "../../include/util.hpp"
#include "../../include/communication.hpp"

/**
 * @brief Retrieves the option passed to the client program
 * @param argc Number of options passed
 * @param argv Pointer to the options passed
 * @param num_cp_server Where to store the number of copies of a server
 * @param service Where to store the number of service to be deployed
 * @param num_options Number of options expected
 * @return None
 */

void get_arg(int32_t argc, char_t *argv[], uint8_t &num_cp_server,
	uint8_t &service, char_t num_options)
{
	char_t c;
	uint8_t cnt_options = 0;

	if (argv[optind] == NULL || argv[optind + 1] == NULL) {
		fprintf(stderr, "Mandatory argument missing!\n");
		exit(EXIT_FAILURE);
	}
	while ((c = getopt(argc, argv, "s:n:")) != -1) {

		cnt_options++;

		switch (c) {			
		case 'n':
			num_cp_server = atoi(optarg);
			break;
		case 's':
			service = atoi(optarg);
			break;
		case '?':
			if (optopt == 't')
				fprintf (stderr, "Option -%c requires "
					"an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option -%c.\n",
						optopt);
			else
				fprintf(stderr, "Unknown option character\n");
			exit(EXIT_FAILURE);
		default:
			exit(EXIT_FAILURE);
		}
	}

	if (cnt_options != num_options) {
		fprintf (stderr, "The number of options must be: %d\n", 
			num_options);
		exit(EXIT_FAILURE);
	}

}

/**
 * @brief Retrieves the option passed to the server program
 * @param argc Number of options passed
 * @param argv Pointer to the options passed
 * @param service Where to store the number of service to be deployed
 * @param num_options Number of options expected
 * @return None
 */

void get_arg(int32_t argc, char_t *argv[], service_type_t &service, 
	char_t num_options)
{
	char_t c;
	uint8_t cnt_options = 0;

	if (argv[optind] == NULL || argv[optind + 1] == NULL) {
		fprintf(stderr, "Mandatory argument missing!\n");
		exit(EXIT_FAILURE);
	}
	while ((c = getopt(argc, argv, "s:n:")) != -1) {

		cnt_options++;

		switch (c) {			
		case 's':
			service = (service_type_t) atoi(optarg);
			break;
		case '?':
			if (optopt == 't')
				fprintf (stderr, "Option -%c requires "
					"an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option -%c.\n",
						optopt);
			else
				fprintf(stderr, "Unknown option character\n");
			exit(EXIT_FAILURE);
		default:
			exit(EXIT_FAILURE);
		}
	}

	if (cnt_options != num_options) {
		fprintf (stderr, "The number of options must be: %d\n", 
			num_options);
		exit(EXIT_FAILURE);
	}

}

/**
 * @brief Adds a socket to the actual context
 * @param ctx Pointer to the actual context
 * @param addr Address of the socket
 * @param port Port of the socket
 * @param skt_type Type of the socket (ZMQ_REP, ZMQ_REQ, etc.)
 * @param dir Direction of the communication (CONNECT or BIND)
 * @return Pointer to the created socket
 */

zmq::socket_t* add_socket(zmq::context_t *ctx, std::string addr, uint16_t port,
	int32_t skt_type, uint8_t dir)
{
	zmq::socket_t *skt;
	std::string p, conf;
	char_t str[MAX_LENGTH_STRING_PORT];
	
	/* Create the ZMQ socket */
	try {
		skt = new zmq::socket_t(*ctx, skt_type);
	} catch (std::bad_alloc& ba) {
		std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	memset(str, '\0', MAX_LENGTH_STRING_PORT);
	sprintf(str, "%d", port);
	p.assign(str);
	conf = (COM_PROTOCOL + addr + ":" + p);
	
	if (dir == BIND)
		skt->bind(conf.c_str());
	else
		skt->connect(conf.c_str());
	
	std::cout << "Configuration: " << conf << std::endl;
	
	return skt; 
}

/**
 * @brief Sends a message composed by multiple frames
 * @param skt Socket used to send the messages
 * @param msg Vector containing the messages to be sent
 */
 
void send_multi_msg(zmq::socket_t *skt, std::vector<zmq::message_t> &msg)
{
	uint8_t i;
	zmq::message_t tmp;
	
	for (i = 0; i < msg.size() - 1; i++) {
		tmp.rebuild(msg[i].data(), msg[i].size());
		skt->send(tmp, ZMQ_SNDMORE);
	}

	/* Last message in the sequence */
	tmp.rebuild(msg[i].data(), msg[i].size());
	skt->send(tmp, 0);
}

 
/**
 * @brief It deploys the server copies for the specified service
 * @param service service It is the server service
 * @param num_copy_server list_server_pid Array of server pid
 * @param list_server_pid list of the servers PIDs
 * @param status Variable to monitor children
 */

void deployment(uint8_t service, uint8_t num_copy_server, 
	pid_t *list_server_pid, int32_t *status)
{
	int8_t ret;
	uint8_t i = 0;

	/* Server copies deployment */
	for (;;) {
		if (i == num_copy_server) {
			std::cout << "#Deployment_Unit: "
				"Server copies deployed" << std::endl;	
			/* Wait on the children */
			wait(&status);
			std::cerr << "Wake up!Something happened to "
				"my children!" << std::endl;
			break;
		}
		/* Start the server process */
		list_server_pid[i] = fork();
		if (list_server_pid[i] == 0) {
			char_t server_service = static_cast<char_t>(service);
			/* Becoming one of the redundant copies */
			ret = execlp("./server", &server_service, &i,
					(char_t *)NULL);
			if (ret == -1) {
				perror("Error execlp on server");
				exit(EXIT_FAILURE);
			}
		} else
			i++;
	}
}

