/*
 * deployment_unit.cpp
 *
 *  Created on: May 9, 2017
 *      Author: matteo
 */
#include <stdio.h>
#include <stdlib.h>
#include "../../include/check_util.hpp"

int32_t main (int32_t argc, char_t* argv[])
{
	uint8_t arg;

	get_arg(argc, argv, &arg);

	printf("%d\n", arg);

	return EXIT_SUCCESS;
}
