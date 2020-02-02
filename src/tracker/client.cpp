#include "client.hpp"
#include "tracker.hpp"

namespace server
{
	client::client(tcp::socket sock, tracker& tracker, asio::io_context& context)
		: sock(std::move(sock)), sock_queue(context), trac(tracker)
	{
		setup_handle();
	}

	void client::setup_handle()
	{
		read_handle();
	}

	bool client::handle_message(common::message& message)
	{
		if (message.id != common::message::id_t::LOG_ME)
		{
			return false;
		}

		username = message.data;
		status = status_t::logged;
		auto alert_new_client = message;
		alert_new_client.id = common::message::id_t::NEW_CLIENT;
		trac.broadcast_message(message);
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

	void client::write_buffer(const std::string& buff)
	{
		asio::post(sock_queue,
			[this, self = shared_from_this(), &buff]()
		{
			asio::async_write(sock, asio::buffer(buff), [this, self, buff](asio::error_code ec, std::size_t size) {
				if (ec)
				{
					trac.remove_bad_client(self);
				}
				});
		});
	}
}
