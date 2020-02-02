#ifndef CLIENT_NETWORK_HPP
#define CLIENT_NETWORK_HPP

#include "asio.hpp"
#include "peer.hpp"
#include <unordered_map>

namespace client
{
	class network
	{
		network();
		void run();

		asio::io_context service;
		asio::signal_set signals;
		std::unordered_map<std::string, peer> peers;
		asio::ip::tcp::socket sock;
	};
	
}

#endif //!CLIENT_CLIENT_HPP