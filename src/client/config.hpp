#ifndef CLIENT_CONFIG_HPP
#define CLIENT_CONFIG_HPP

#include <string>
#include "message.hpp"

namespace Json
{
	class Value;
}


namespace client 
{
	struct config 
	{
		config()
		{
			listening_port = randomize_port();
		}

		std::string username;
		int thread_number = 4;
		std::string server_ip = "localhost";
		std::string server_port = "8752";
		int listening_port;
		int second_before_server_retry = 30;

		static config config_from_json(const Json::Value& val);
		static void config_to_json(const config& config, Json::Value& val);
		static int randomize_port();
		static common::message log_me_from_config(const config& config);
	};
}

#endif //!CLIENT_CONFIG_HPP