
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <cstddef>
#include "asio.hpp"
#include "tracker_config.hpp"
#include "client.hpp"

namespace common
{
	struct message;
}

namespace server
{
	class tracker
	{
	public:
		tracker(const tracker_config& config);

		void run(std::size_t thread_num = 1);
		void remove_bad_client(const std::shared_ptr<client>& cli);
		void broadcast_message(const common::message& mess);
		void sent_all_connection_info_to(const std::shared_ptr<client>& cli);

	private:
		tracker_config config;

		asio::io_context network_service;
		asio::signal_set signals;
		asio::ip::tcp::acceptor acceptor;
		asio::ip::tcp::socket sock;


		std::vector<std::shared_ptr<client>> current_connection;
		asio::io_context::strand connection_strand;

		constexpr static std::size_t max_thread = 32;

		void start_accept();
		void setup_client(asio::ip::tcp::socket s);
	};
}
#endif // !TRACKER_HPP