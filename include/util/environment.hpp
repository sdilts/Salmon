#pragma once

#include <filesystem>
#include <exception>
#include <optional>
#include <sstream>

namespace salmon {

	struct ConfigException : public std::runtime_error {
		ConfigException(const std::string &msg=""): std::runtime_error(msg) {}
	};

	/**
	 * Returns the value of the given environment variable, or an empty string
	 * if it is not set.
	 **/
	std::optional<std::string> read_env(const std::string &name);

	/**
	 * Get the path for the data directory as per the <a href="https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html">XDG spec</a>
	 **/
	std::filesystem::path get_data_dir();

	/**
	 * Get the path for the config directory as per the <a href="https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html">XDG spec</a>
	 **/
	std::filesystem::path get_config_dir();

	/**
	 * Get the path for the cache directory as per the <a href="https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html">XDG spec</a>
	 **/
	std::filesystem::path get_cache_dir();

}
