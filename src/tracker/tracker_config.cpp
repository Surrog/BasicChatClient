
#include "tracker_config.hpp"
#include "json/json.h"

namespace server
{
	tracker_config tracker_config::tracker_config_from_json(const Json::Value& val)
	{
		tracker_config result;

		result.port = val.get("port", result.port).asInt();
		result.anonymous_client_timeout = val.get("anonymous_client_timeout", result.anonymous_client_timeout).asInt();
		result.thread_number = val.get("thread_number", result.thread_number).asInt();

		return result;
	}

	void tracker_config::tracker_config_to_json(const tracker_config& config, Json::Value& val)
	{
		val["port"] = config.port;
		val["anonymous_client_timeout"] = config.anonymous_client_timeout;
		val["thread_number"] = config.thread_number;
	}
}