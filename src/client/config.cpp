#include "config.hpp"
#include "json/json.h"

#include <cstdlib>
#include <random>

namespace client 
{
	config config::config_from_json(const Json::Value& val)
	{
		config result;

		result.thread_number = val.get("thread_number", result.thread_number).asInt();
		result.username = val.get("username", "").asString();
		result.server_ip = val.get("server_ip", result.server_ip).asString();
		result.server_port = val.get("server_port", result.server_port).asString();
		result.listening_port = val.get("listening_port", result.listening_port).asInt();
		result.second_before_server_retry = val.get("second_before_server_retry", result.second_before_server_retry).asInt();

		return result;
	}

	void config::config_to_json(const config& config, Json::Value& val)
	{
		val["thread_number"] = config.thread_number;
		val["username"] = config.username;
		val["server_ip"] = config.server_ip;
		val["server_port"] = config.server_port;
		val["listening_port"] = config.listening_port;
		val["second_before_server_retry"] = config.second_before_server_retry;
	}

	int config::randomize_port()
	{
		std::random_device generator;
		std::uniform_int_distribution<int> distribution(1001, 9999);
		int val = distribution(generator);
		return val;
	}

	common::message config::log_me_from_config(const config& config)
	{
		common::message result;
		result.id = common::message::id_t::LOG_ME;
		result.data = config.username;
		result.port = std::to_string(config.listening_port);
	
		return result;
	}
}
