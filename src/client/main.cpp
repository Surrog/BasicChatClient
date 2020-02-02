
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "config.hpp"
#include "json/json.h"
#include "main.hpp"

namespace client
{
	main::main(config conf)
		: service(), signals(service, SIGINT, SIGTERM), peers(), server_sock(service), conf(conf)
	{
		signals.async_wait([this](auto, auto) {
			service.stop();
			});
	}

	void main::run()
	{
		std::vector<std::thread> thds;

		for (std::size_t i = 0; i < conf.thread_number - 1; i++)
		{
			thds.emplace_back([this]() {
				service.run();
				});
		}
		thds.emplace_back([this]() {
			service.run();
			});

		for (auto& t : thds)
		{
			t.join();
		}

	}
}

int main(int argc, char** argv)
{
	std::filesystem::path config_path = "tracker_config.json";
	client::config config;

	if (std::filesystem::exists(config_path))
	{
		Json::CharReaderBuilder builder;
		std::ifstream stream(config_path);
		builder["collectComments"] = false;
		Json::Value json_config;
		JSONCPP_STRING errs;
		if (Json::parseFromStream(builder, stream, &json_config, &errs))
		{
			config = client::config::config_from_json(json_config);
		}
		else
		{
			std::cout << "error reading config file " << config_path << " : " << errs << '\n';
		}
	}
	else if (config.username.empty())
	{
		std::cout << "config path doesn't exist, please enter your username, char limit 256: ";
		std::string val;

		while (val.size() == 0 && val.size() < 256)
		{
			std::cin >> val;
			if (val.size() > 256)
			{
				std::cout << "username too large, please enter a valid username, char limit 256\n";
			}
		}

		config.username = val;
	}

	client::main m(config);

	m.run();
}
