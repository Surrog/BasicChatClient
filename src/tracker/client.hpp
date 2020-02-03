#ifndef TRACKER_CLIENT_HPP
#define TRACKER_CLIENT_HPP

#include <string>
#include <array>
#include <memory>

#include "asio/ip/tcp.hpp"
#include "asio/strand.hpp"
#include "message.hpp"

namespace server
{
	class tracker;

	class client : public std::enable_shared_from_this<client>
	{
	public:
		enum class status_t {
			anonymous,
			logged,
			broadcasted,
			errored
		};

		std::atomic<status_t> status = status_t::anonymous;
	private:
		using tcp = asio::ip::tcp;

		tcp::socket sock;
		asio::io_context::strand sock_queue;

		std::string username;
		std::string listening_port;
		std::array<char, 1024> buffer;
		tracker& trac;
		common::message mess;

	public:
		client(tcp::socket sock, tracker& trac, asio::io_context& context);

		void write_buffer(std::string buff);
		void setup_handle();

	private:
		void read_handle();
		bool handle_message(common::message& message);
	};
}
#endif //!TRACKER_CLIENT_HPP
