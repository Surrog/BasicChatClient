
#include "asio.hpp"
#include "json/json.h"

#include "tracker.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
	std::cout << "server started\n";
	server::tracker_config config;

	std::filesystem::path config_path = "tracker_config.json";
	if (argc > 1) { config_path = argv[1]; }

	
	if (std::filesystem::exists(config_path))
	{
		Json::CharReaderBuilder builder;
		std::ifstream stream(config_path);
		builder["collectComments"] = false;
		Json::Value json_config;
		JSONCPP_STRING errs;
		if (Json::parseFromStream(builder, stream, &json_config, &errs))
		{
			config = server::tracker_config::tracker_config_from_json(json_config);
		}
		else
		{
			std::cout << "error reading config file " << config_path << " : " << errs << '\n';
		}
	}
	else
	{
		std::cout << "config file " << config_path << " doesn't exist, creating a new one with default values\n";
		Json::Value json_config;
		server::tracker_config::tracker_config_to_json(config, json_config);
		std::ofstream stream(config_path, std::ios::trunc);
		stream << json_config;
	}

	server::tracker serv(config);
	serv.run(config.thread_number);
	return 0;
}