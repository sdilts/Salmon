#ifndef SALMON_ARRAY
#define SALMON_ARRAY

#include <stdbool.h>
#include <stdlib.h>

struct salmon_array {
	size_t length;
	void *data;
};

struct salmon_vector {
	struct salmon_array array;
	size_t actual_size;
};

#endif
