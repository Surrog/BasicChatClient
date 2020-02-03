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
		void peer_lookout_and_send_message(const std::string username, const std::string& message);
		void register_connected_client(const std::shared_ptr<peer>& cli, bool was_known);
		void cleanup_failed_client(const std::shared_ptr<peer>& cli);
		void run();

		asio::io_context service;

	private:
		//configuration
		config conf;
		asio::signal_set signals;
		static constexpr int max_thread = 16;

		//peer network
		asio::ip::tcp::socket new_connection;
		asio::ip::tcp::acceptor incoming_connection;
		std::unordered_map<std::string, std::shared_ptr<peer>> connected_peers;
		std::unordered_map<std::string, std::shared_ptr<peer>> known_peers;
		asio::io_context::strand peer_strand;

		//tracker network
		asio::ip::tcp::socket server_sock;
		std::string server_buff;
		common::message server_mess;

		//private impl
		void handle_commandline();
		void start_accept();
		void setup_server_read();
		bool handle_message(const common::message& mess);
	};
	
}

#endif //!CLIENT_MAIN_HPP