#ifndef SALMON_STRING
#define SALMON_STRING

#include <string.h>

struct salmon_string {
	size_t length;
	char *data;
};

#endif
