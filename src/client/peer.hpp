#ifndef CLIENT_PEER_HPP
#define CLIENT_PEER_HPP

#include "asio.hpp"
#include "message.hpp"

namespace client
{
	class main;

	class peer : public std::enable_shared_from_this<peer>
	{
	public:
		peer(main& service, std::string ip, unsigned short port, std::string id = "");
		peer(main& service);
		void write_buffer(const std::string& buffer);
		void write_buffer(const common::message& mess);
		void setup_read();
		bool handle_message(const common::message& mess);
		void connect_and_send_message(const common::message& login, const common::message& mess, bool is_known);

		std::string user;
		asio::ip::tcp::socket sock;

	private:
		//logins
		std::string ip;
		unsigned short port;

		//plumbing
		main& track;

		//network
		std::array<char, 1024> read_buff;
		common::message read_mess;
		asio::io_context::strand write_strand;
	};
}

#endif //!CLIENT_PEER_HPP