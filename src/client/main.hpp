#ifndef CLIENT_MAIN_HPP
#define CLIENT_MAIN_HPP

#include <memory>
#include <unordered_map>
#include "asio.hpp"
#include "peer.hpp"
#include "config.hpp"

namespace client
{
	class main
	{
	public:
		main(config conf);
		void start_accept();
		void peer_lookout_and_send_message(const std::string username, const std::string& message);
		void register_connected_client(const std::shared_ptr<peer>& cli, bool was_known);
		void cleanup_failed_client(const std::shared_ptr<peer>& cli);
		void run();

		asio::io_context service;

	private:
		asio::signal_set signals;
		asio::ip::tcp::socket server_sock;
		asio::ip::tcp::socket new_connection;
		asio::ip::tcp::acceptor incoming_connection;
		config conf;

		std::unordered_map<std::string, std::shared_ptr<peer>> connected_peers;
		std::unordered_map<std::string, std::shared_ptr<peer>> known_peers;
		asio::io_context::strand peer_strand;

		static constexpr int max_thread = 16;
	};
	
}

#endif //!CLIENT_MAIN_HPP