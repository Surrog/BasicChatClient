#ifndef COMMON_MESSAGE_HPP
#define COMMON_MESSAGE_HPP

#include <cstdint>
#include <array>
#include <vector>

namespace common
{
	struct message 
	{
		enum class id_t : std::int32_t
		{
			LOG_ME = 0
			, NEW_CLIENT
			, MESSAGE
			, SIZE //Keep this in the last place
		};

		id_t id;
		std::string ip;
		std::string data;

		static constexpr std::size_t log_me_max_data = 256;
		static constexpr std::size_t message_max_data = 512;

		static bool serialize(const message& mess, std::string& in);
		static bool deserialize(const char* beg, const char* end, message& in);
	};
	
}

#endif // !COMMON_MESSAGE_HPP