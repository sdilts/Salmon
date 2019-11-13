#include <filesystem>
#include <exception>
#include <sstream>

#include "util/environment.hpp"

namespace salmon {

	/**
	 * Returns the value of the given environment variable, or an empty string
	 * if it is not set.
	 **/
	std::string read_env(const std::string &name) {
		char const* tmp = getenv(name.c_str());
		if (tmp == nullptr) {
			return std::string();
		} else {
			return std::string(tmp);
		}
	}

	/**
	 * Helper function to return a directory prefixed the by the value
	 * of an environment variable or another path.
	 *
	 * @see get_data_dir
	 * @see get_config_dir
	 * @see get_cache_dir
	 * @param var_name the environment variable
	 * @param other_path the path to use if the environment variable isn't set.
	 **/
	static std::filesystem::path env_or_string(const std::string &var_name,
											   const std::string &other_path) {
		std::filesystem::path target_dir;
		std::string xdg_dir = read_env(var_name);
		if(!xdg_dir.empty()) {
			target_dir = xdg_dir;
		} else {
			std::string home_dir = read_env("HOME");
			if (!home_dir.empty()) {
				target_dir = home_dir;
				target_dir /= other_path;
			} else {
				std::ostringstream error_message;
				error_message << "Could not get config dir from $" <<
					var_name << " or $HOME. Check to ensure either of these environment variables are set.";
				throw ConfigException(error_message.str());
			}
		}
		target_dir /= "salmon";
		return target_dir;
	}

	std::filesystem::path get_data_dir() {
		return env_or_string("XDG_DATA_HOME", ".local/share");
	}

	std::filesystem::path get_config_dir() {
		return env_or_string("XDG_CONFIG_HOME", ".config");
	}

	std::filesystem::path get_cache_dir() {
		return env_or_string("XDG_CACHE_DIR", ".cache");
	}
}
