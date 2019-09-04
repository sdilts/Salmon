#ifndef SALMON_CORE_SYMBOL
#define SALMON_CORE_SYMBOL

#include <salmon/core/salmon_string.h>

struct salmon_symbol {
	struct salmon_string package;
	struct salmon_string name;
};

#endif
