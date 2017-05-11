/*
 * check_util.cpp
 *
 *  Created on: May 9, 2017
 *      Author: matteo
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "../../include/types.hpp"



/*
 * @brief Retrieves the option passed to the program
 * @param argc Number of options passed
 * @param argv Pointer to the options passed
 * @param num_cp_server Where to store the number of copies of a server
 * @param service Where to store the number of service to be deployed
 * @retval None
 */

void get_arg(int32_t argc, char_t *argv[], char_t *num_cp_server,
		char_t *service, char_t num_options)
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
			*num_cp_server = atoi(optarg);
			break;
		case 's':
			*service = atoi(optarg);
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
