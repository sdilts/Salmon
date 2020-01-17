#include <salmon/config.hpp>
#include <system_error>
#include <iostream>

namespace salmon {

	/**
	 * If the given path doesn't exist, create it.
	 * @param path ensure the given directory path exists.
	 * @param If there is an error, the code will be placed in this spot
	 * @return if the directory could not be created.
	 **/
	static bool ensure_directory(const std::filesystem::path &path, std::error_code &err) {
		if(!std::filesystem::exists(path)) {
			return std::filesystem::create_directories(path, err);
		} else if(!std::filesystem::is_directory(path)) {
			err = std::make_error_code(std::errc::operation_not_permitted);
			return false;
		}
		return true;
	}

	std::optional<std::error_code> Config::ensure_required_dirs(const Config &config) {
		std::error_code error;
		if(!ensure_directory(config.data_dir, error)) {
			std::cerr << "Could not create data directory: " << config.data_dir
					  << ": " << error.message() << "\n";
			return std::make_optional(error);
		}
		return std::nullopt;
	}

}

std::ostream& operator<<(std::ostream &os, const salmon::Config& config) {
	return os << "Verbosity level: " << config.verbosity_level
			  << "\nDirectory paths:"
			  << "\n  Cache:  " << config.cache_dir
			  << "\n  Config: " << config.config_dir
			  << "\n  Data:   " << config.data_dir << "\n";
}
