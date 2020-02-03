
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "config.hpp"
#include "json/json.h"
#include "main.hpp"
#include "peer.hpp"
#include "message.hpp"

int main(int argc, char** argv)
{
	std::filesystem::path config_path = "client_config.json";
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
		std::ofstream stream(config_path);
		Json::Value json_config;
		client::config::config_to_json(config, json_config);
		stream << json_config;
	}

	std::cout << "usage:\n\t" << R"_(`<user>|<message>`)_" << "\n\t" << R"_(`users` will get the list of currently logged users)_" << '\n';
	std::cout << '\t' << R"_(example: `toto|hello world !`)_" << "\n\t" << R"_(example: `localhost:1337|comment est votre blanquette ?)_" << '\n';
	client::main m(config);

	m.run();
}
