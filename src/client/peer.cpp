#include "peer.hpp"
#include "message.hpp"

namespace client
{
	peer::peer(asio::ip::tcp::socket sock, asio::io_context& service)
		: sock(std::move(sock)), strand(service)
	{

	}

	void peer::write_buffer(const common::message& mess)
	{
		common::message::serialize(mess, buffer);

		asio::post(strand, [this, buf = buffer]() {
			asio::write(sock, asio::buffer(buf));
			});
	}
}