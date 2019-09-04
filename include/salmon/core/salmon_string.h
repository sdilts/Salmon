#ifndef SALMON_CORE_STRING
#define SALMON_CORE_STRING

#include <string.h>

struct salmon_string {
	size_t length;
	char *data;
};

#endif
