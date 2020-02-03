#include "client.hpp"
#include "tracker.hpp"

namespace server
{
	client::client(tcp::socket sock, tracker& tracker, asio::io_context& context)
		: sock(std::move(sock)), sock_queue(context), trac(tracker)
	{}

	void client::setup_handle()
	{
		read_handle();
	}

	common::message client::client_info() const
	{
		common::message result;

		result.id = common::message::id_t::NEW_CLIENT;
		result.ip = sock.local_endpoint().address().to_string();
		result.data = username;
		result.port = listening_port;

		return result;
	}

	bool client::handle_message(common::message& message)
	{
		if (message.id != common::message::id_t::LOG_ME)
		{
			return false;
		}

		username = message.data;
		listening_port = message.port;
		status = status_t::logged;
		
		trac.broadcast_message(client_info());
		trac.sent_all_connection_info_to(shared_from_this());

		if (status != status_t::errored)
		{
			status = status_t::broadcasted;
		}
		return true;
	}

	void client::read_handle()
	{
		sock.async_read_some(
			asio::buffer(buffer)
			, [this, self = shared_from_this()](asio::error_code ec, std::size_t size)
		{
			if (ec || !common::message::deserialize(buffer.data(), buffer.data() + size, mess) || !handle_message(mess))
			{
				trac.remove_bad_client(self);
				return;
			}
			read_handle();
		});
	}

	void client::write_buffer(std::string buff)
	{
		asio::post(sock_queue,
			[this, self = shared_from_this(), buff]()
		{
			asio::error_code ec;
			asio::write(sock, asio::buffer(buff), ec);
			if (ec)
			{
				trac.remove_bad_client(self);
			}
		});
	}
}
