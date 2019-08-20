#include <config.hpp>

std::ostream& operator<<(std::ostream &os, const salmon::CompilerConfig& config) {
	return os << "Verbosity level: " << config.verbosity_level
			  << "\nDirectory paths:"
			  << "\n  Cache:  " << config.cache_dir
			  << "\n  Config: " << config.config_dir
			  << "\n  Data:   " << config.data_dir << "\n";
}
