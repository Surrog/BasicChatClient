#include "peer.hpp"
#include "message.hpp"
#include <iostream>

#include "main.hpp"

namespace client
{
	peer::peer(main& m, std::string ip, unsigned short port)
		:track(m), sock(m.service), write_strand(m.service), ip(ip), port(port), read_buff()
	{}

	peer::peer(main& m, asio::ip::tcp::socket sock)
		: track(m), sock(std::move(sock)), write_strand(m.service), read_buff()
	{}

	void peer::write_buffer(const std::string& buffer)
	{
		asio::post(write_strand, [this, buf = buffer]() { asio::write(sock, asio::buffer(buf)); });
	}

	void peer::write_buffer(const common::message& mess)
	{
		std::string buff;
		common::message::serialize(mess, buff);

		write_buffer(buff);
	}

	void peer::setup_read()
	{
		sock.async_read_some(
			asio::buffer(read_buff)
			, [this, self = shared_from_this()](asio::error_code ec, std::size_t size)
		{
			if (ec || !common::message::deserialize(read_buff.data(), read_buff.data() + size, read_mess) || !handle_message(read_mess))
			{
				std::cout << "error on peer " << id << ':' << port << std::endl;
				sock.close();
				return;
			}
			setup_read();
		});
	}

	bool peer::handle_message(const common::message& mess)
	{
		if (id.empty() && mess.id != common::message::id_t::LOG_ME)
		{
			return false;
		}

		if (mess.id == common::message::id_t::LOG_ME)
		{
			id = mess.data;
			ip = sock.local_endpoint().address().to_string();
			port = sock.local_endpoint().port();
		}

		if (mess.id == common::message::id_t::MESSAGE)
		{
			std::cout << id << ':' << mess.data << std::endl;
		}

		return true;
	}

	void peer::connect_and_send_message(const common::message& login, const common::message& mess, bool is_known)
	{
		asio::ip::tcp::resolver resolver(track.service);
		asio::error_code ec;
		auto eps = resolver.resolve(ip, std::to_string(port), ec);

		asio::async_connect(sock, eps,
			[this, eps, login, mess, is_known, self = shared_from_this()](asio::error_code ec, asio::ip::tcp::endpoint ep) {
			if (ec)
			{
				std::cout << "failed to connect to " << ep.address().to_string() << ':' << ep.port() << std::endl;
				if (is_known)
				{
					track.cleanup_failed_client(self);
				}
				return;
			}

			setup_read();
			write_buffer(login);
			write_buffer(mess);
		});
	}
}