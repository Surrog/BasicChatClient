#include "peer.hpp"

peer::peer(asio::ip::tcp::socket sock)
	: sock(std::move(sock))
{
	
}
