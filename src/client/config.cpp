#include "config.hpp"
#include "json/json.h"

namespace client 
{
	config config::config_from_json(const Json::Value& val)
	{
		config result;

		result.thread_number = val.get("thread_number", result.thread_number).asInt();
		result.username = val.get("username", "").asString();
		result.server_ip = val.get("server_ip", result.server_ip).asInt();
		result.server_port = val.get("server_port", result.server_port).asInt();

		return result;
	}

	void config::config_to_json(const config& config, Json::Value& val)
	{
		val["thread_number"] = config.thread_number;
		val["username"] = config.username;
		val["server_ip"] = config.server_ip;
		val["server_port"] = config.server_port;
	}
}
