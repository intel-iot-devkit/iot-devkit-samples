/*
*  Copyright (c) 2018 Intel Corporation.
*  Licensed under the MIT license. See LICENSE file in the project root for full license information.
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("Hello, Cmake!\n");
	fflush(stdout);
	sleep(1);
	printf("Goodbye!\n");
	return 0;
}
