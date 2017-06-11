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
#include <fstream>
#include <time.h>
#include <sys/stat.h>
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
	
	if (port != 0)
		conf = (TCP_PROTOCOL + addr + ":" + p);
	else
		conf = (IPC_PROTOCOL + addr);

	if (dir == BIND)
		skt->bind(conf.c_str());
	else
		skt->connect(conf.c_str());
	
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
		skt->send(tmp, ZMQ_SNDMORE | ZMQ_DONTWAIT);
	}

	/* Last message in the sequence */
	tmp.rebuild(msg[i].data(), msg[i].size());
	skt->send(tmp, 0 | ZMQ_DONTWAIT);
}



/**
 * @brief Copies the timestruct
 * @param dst destination
 * @param src source
 */

void time_copy(struct timespec *dst, const struct timespec *src)
{
	dst->tv_sec = src->tv_sec;
	dst->tv_nsec = src->tv_nsec;
}

/**
 * @brief Adds milliseconds to the given time 
 * @param dst destination
 * @param ms amout of milliseconds
 */
 
void time_add_ms(struct timespec *dst, long int ms)
{
	dst->tv_sec += ms / 1000;
	dst->tv_nsec += (ms % 1000) * 1e6;
	if (dst->tv_nsec > 1e9) {
		dst->tv_nsec -= 1e9;
		dst->tv_sec++;
	}
}

/**
 * @brief Adds nanoseconds to the given time 
 * @param dst destination
 * @param ms amout of nanoseconds
 */

void time_add_ns(struct timespec *dst, long int ns)
{
	dst->tv_nsec += ns; // % (1000 * 1000);
	if(dst->tv_nsec > 1e9) {
		dst->tv_nsec -= 1e9;
		dst->tv_sec++;
	}
}


/**
 * @brief Compares two times
 * @param t1 
 * @param t2
 * @return It returns 1 if the first time passed is greater than the second one
 * 	   0 if times are equal, else -1
 */
 
int32_t time_cmp(struct timespec *t1, struct timespec *t2)
{
	if (t1->tv_sec > t2->tv_sec)
		return 1;
	if (t1->tv_sec < t2->tv_sec)
		return -1;
	if (t1->tv_nsec > t2->tv_nsec)
		return 1;
	if (t1->tv_nsec < t2->tv_nsec)
		return -1;
	
	return 0;
}

/**
 * @brief It does a busy wait
 * @param ms amount of milliseconds of busy wait
 */
 
void busy_wait(uint32_t ms)
{
	struct timespec t, now;

	clock_gettime(CLOCK_MONOTONIC, &t);
	time_add_ms(&t, ms);
	do {
		clock_gettime(CLOCK_MONOTONIC, &now);
	} while(time_cmp(&now, &t) < 0);
}

/**
 * @brief Writes a log row to an output stream (either the console or a file)
 * @param who String representing who is writing the log row
 * @param what Information to log
 */

void write_log(std::string who, std::string what)
{
	tm *now_struct;
	struct timespec now;
	
	clock_gettime(CLOCK_REALTIME, &now);
	now_struct = localtime(&now.tv_sec);
	
#ifdef CONSOLE_LOG
	
	std::cout << now_struct->tm_year + ABS_YEAR << "-" << 
	++now_struct->tm_mon <<
	"-" << now_struct->tm_mday << "_" << now_struct->tm_hour << ":"
	<< now_struct->tm_min << ":" << now_struct->tm_sec << "." << 
	(int32_t)(now.tv_nsec / 1e6) << " " << who << " " << what <<
	std::endl;
#else
	struct stat sb;

	/* Check if the 'log/' directory already exists */
	if (stat("log/", &sb) != 0 || !S_ISDIR(sb.st_mode)) {
		mkdir("log/", 0777);
	}
	std::filebuf fb;
	fb.open("log/" + who + ".txt", std::ios::out | std::ios::app);
	std::ostream os(&fb);
	
	os << now_struct->tm_year + ABS_YEAR << "-" << 
	++now_struct->tm_mon <<
	"-" << now_struct->tm_mday << "_" << now_struct->tm_hour << ":"
	<< now_struct->tm_min << ":" << now_struct->tm_sec << "." << 
	(int32_t)(now.tv_nsec / 1e6) << " " << who << " " << what <<
	std::endl;
	
	fb.close();
#endif
}