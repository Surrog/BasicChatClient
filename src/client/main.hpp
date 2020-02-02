#ifndef CLIENT_MAIN_HPP
#define CLIENT_MAIN_HPP

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
		std::unordered_map<std::string, peer> peers;
		asio::ip::tcp::socket server_sock;
		config conf;
	};
	
}

#endif //!CLIENT_MAIN_HPP