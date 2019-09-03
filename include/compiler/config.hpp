#ifndef SALMON_CONFIG
#define SALMON_CONFIG

#include <filesystem>
#include <ostream>

namespace salmon {

	struct CompilerConfig {
		int verbosity_level;
		const std::filesystem::path cache_dir;
		const std::filesystem::path config_dir;
		const std::filesystem::path data_dir;
	};
}

std::ostream& operator<<(std::ostream &os, const salmon::CompilerConfig& config);

#endif
