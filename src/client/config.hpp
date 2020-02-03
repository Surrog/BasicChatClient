#ifndef CLIENT_CONFIG_HPP
#define CLIENT_CONFIG_HPP

#include <string>

namespace Json
{
	class Value;
}


namespace client 
{
	struct config 
	{
		std::string username;
		int thread_number = 4;
		std::string server_ip = "localhost";
		std::string server_port = "8752";
		int listening_port = randomize_port();

		static config config_from_json(const Json::Value& val);
		static void config_to_json(const config& config, Json::Value& val);
		static int randomize_port();
	};
}

#endif //!CLIENT_CONFIG_HPP