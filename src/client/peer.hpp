#ifndef CLIENT_PEER_HPP
#define CLIENT_PEER_HPP

#include "asio.hpp"

namespace common
{
	struct message;
}

namespace client
{
	class peer : public std::enable_shared_from_this<peer>
	{
	public:
		peer(asio::ip::tcp::socket sock, asio::io_context& service);
		void write_buffer(const common::message& mess);

	private:
		std::string id;
		std::string ip;
		std::string port;

		std::string buffer;
		asio::ip::tcp::socket sock;
		asio::io_context::strand strand;
	};
}

#endif //!CLIENT_PEER_HPP