#ifndef TRACKER_CONFIG
#define TRACKER_CONFIG

#include <cstdint>

namespace Json
{
	class Value;
}

namespace server
{
	struct tracker_config
	{
		int port = 8752; // port listened
		int anonymous_client_timeout = 10; // time in second before a client is kicked out
		int thread_number = 4;

		static tracker_config tracker_config_from_json(const Json::Value& val);
		static void tracker_config_to_json(const tracker_config& config, Json::Value& val);

	};
}
#endif //!TRACKER_CONFIG