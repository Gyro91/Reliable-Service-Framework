/*
 * check_util.cpp
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "../../include/communication.hpp"
#include "../../include/util.hpp"


/**
 * @brief Retrieves the option passed to the program
 * @param argc Number of options passed
 * @param argv Pointer to the options passed
 * @param num_cp_server Where to store the number of copies of a server
 * @param service Where to store the number of service to be deployed
 * @param num_options Number of options expected
 * @return None
 */

void get_arg(int32_t argc, char_t *argv[], uint8_t &num_cp_server,
		service_type_t &service, char_t num_options)
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
			service = static_cast<service_type_t>(atoi(optarg));
			break;
		case '?':
			if (optopt == 't')
				fprintf (stderr,
						"Option -%c requires "
						"an argument.\n",
						optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option -%c.\n",
						optopt);
			else
				fprintf (stderr,
						"Unknown option character\n");
			exit(EXIT_FAILURE);
		default:
			exit(EXIT_FAILURE);
		}
	}

	if (cnt_options != num_options) {
		fprintf (stderr,
				"The number of options must be: %d\n",
				num_options);
		exit(EXIT_FAILURE);
	}

}

/**
 * @brief 
 * @param addr
 * @param port
 * @param type
 * @return 
 */

zmq::socket_t* add_socket(zmq::context_t * ctx, std::string addr, uint16_t port, 
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
	
	std::cout << "Configuration: "<< conf << std::endl;
	
	return skt; 
}

void send_multi_msg(zmq::socket_t* skt, std::vector<zmq::message_t> msg)
{
	uint8_t i;
	for (i = 0; i < msg.size() - 1; i++)
		skt->send(msg[i], ZMQ_SNDMORE);

	/* Last message in the sequence */	
	skt->send(msg[++i], 0);
}

void push_msg(std::vector<zmq::message_t> vect, zmq::message_t msg)
{
	zmq::message_t tmp;
	
	
}
