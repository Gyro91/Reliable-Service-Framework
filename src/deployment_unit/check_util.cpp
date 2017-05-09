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
 * This function checks the options passed to the program and eventually
 * stores in the ret argument.
 *
 */

void get_arg(int32_t argc, char_t *argv[], uint8_t *ret)
{
	char_t c;

	if (argv[optind] == NULL || argv[optind + 1] == NULL) {
		fprintf(stderr, "Mandatory argument missing!\n");
		exit(-1);
	}
	while ((c = getopt(argc, argv, "n:")) != -1) {
		switch (c) {
		case 'n':
			*ret = atoi(optarg);
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
			exit(-1);
		default:
			exit(-1);
		}
	}
}
