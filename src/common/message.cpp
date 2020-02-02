#include "message.hpp"
#include "json/json.h"
#include <string_view>

namespace common 
{
	bool message::serialize(const message& mess, std::string& in)
	{
		if (mess.data.empty())
		{
			return false;
		}

		if (mess.id == id_t::LOG_ME && mess.data.size() > log_me_max_data)
		{
			return false;
		}

		if (mess.id == id_t::MESSAGE && mess.data.size() > message_max_data)
		{
			return false;
		}

		Json::Value val;
		val["id"] = static_cast<std::int32_t>(mess.id);
		val["data"] = mess.data;
		
		in = val.toStyledString();
		return true;
	}

	bool message::deserialize(const char* beg, const char* end, message& in)
	{
		static thread_local std::unique_ptr<Json::CharReader> reader = []() {
			Json::CharReaderBuilder builder;
			return std::unique_ptr<Json::CharReader>(builder.newCharReader());
		}();

		Json::Value result;
		Json::String errs;

		if (!reader->parse(beg, end, &result, &errs))
		{
			return false;
		}

		std::int32_t received_id = result.get("id", -1).asInt();

		if (received_id < 0
			|| !(received_id >= static_cast<std::int32_t>(message::id_t::LOG_ME)
				&& received_id < static_cast<std::int32_t>(message::id_t::SIZE)))
		{
			return false;
		}

		auto data = result.get("data", "").asString();
		if (data.empty())
		{
			return false;
		}

		in.id = static_cast<message::id_t>(received_id);
		in.data = data;
		return true;
	}
}
