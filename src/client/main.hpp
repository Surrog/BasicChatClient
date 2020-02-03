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
		void run();

	private:
		asio::io_context service;
		asio::signal_set signals;
		asio::ip::tcp::socket server_sock;
		config conf;

		std::unordered_map<std::string, std::shared_ptr<peer>> peers;
		asio::io_context::strand peers_strand;
	};
	
}

#endif //!CLIENT_MAIN_HPP