#ifndef CLIENT_PEER_HPP
#define CLIENT_PEER_HPP

#include "asio.hpp"

struct peer
{
	peer(asio::ip::tcp::socket sock);

	std::string id;
	std::string ip;

	std::vector<std::string> message;
	asio::ip::tcp::socket sock;
};

#endif //!CLIENT_PEER_HPP