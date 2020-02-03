
#include <iostream>
#include <string>
#include "main.hpp"
#include "asio.hpp"

namespace client
{
	main::main(config conf)
		: service(), signals(service, SIGINT, SIGTERM)
		, server_sock(service), new_connection(service)
		, incoming_connection(service), conf(conf)
		, connected_peers(), known_peers()
		, peer_strand(service), retry_timer(service)

	{
		signals.async_wait([this](auto, auto) {
			service.stop();
			});
	}

	void main::start_accept()
	{
		auto new_peer = std::make_shared<peer>(*this);
		incoming_connection.async_accept(new_peer->sock, [this, new_peer](asio::error_code ec) {
			new_peer->setup_read();
			new_peer->write_buffer(config::log_me_from_config(conf));
			start_accept();
			});
	}

	void main::setup_server_read()
	{
		server_sock.async_read_some(
			asio::buffer(server_buff)
			, [this](asio::error_code ec, std::size_t size)
		{
			if (ec || !common::message::deserialize(server_buff.data(), server_buff.data() + size, server_mess) || !handle_server_message(server_mess))
			{
				std::cerr << "error on server " << ec << " message size: " << size << std::endl;
				server_sock.close();
				return;
			}
			setup_server_read();
		});
	}

	bool main::handle_server_message(const common::message& mess)
	{
		if (mess.id != common::message::id_t::NEW_CLIENT)
		{
			return false;
		}

		if (mess.data == conf.username)
		{
			return true;
		}

		asio::post(peer_strand, [mess, this]()
			{
				auto it = connected_peers.find(mess.data);
				if (it == connected_peers.end())
				{
					known_peers[mess.data] = std::make_shared<peer>(*this, mess.ip
						, static_cast<unsigned short>(std::stoi(mess.port)), mess.data);
				}
			});

		return true;
	}

	void main::peer_lookout_and_send_message(const std::string username, const std::string& message)
	{
		common::message mess;
		mess.id = common::message::id_t::MESSAGE;
		mess.data = message;

		auto it = connected_peers.find(username);
		if (it != connected_peers.end())
		{
			it->second->write_buffer(mess);
			return;
		}

		common::message login = config::log_me_from_config(conf);

		it = known_peers.find(username);
		if (it != known_peers.end())
		{
			it->second->connect_and_send_message(login, mess, true);
			return;
		}

		auto pos = username.find(':'); //port delimiter
		if (pos != std::string::npos)
		{
			auto address = username.substr(0, pos);
			auto port = username.substr(pos + 1);
			auto pe = std::make_shared<peer>(*this, address, std::stoi(port));
			pe->connect_and_send_message(login, mess, false);
			return;
		}

		std::cout << "Could not find user: " << username << '\n';
	}

	void main::register_connected_client(const std::shared_ptr<peer>& cli, bool is_known)
	{
		asio::post(peer_strand, [this, cli, is_known] {
			if (is_known)
			{
				known_peers.erase(cli->user);
			}
			connected_peers[cli->user] = cli;
			});
	}

	void main::cleanup_failed_client(const std::shared_ptr<peer>& cli)
	{
		asio::post(peer_strand, [this, cli] {
			known_peers.erase(cli->user);
			connected_peers.erase(cli->user);
			});
	}

	void main::handle_commandline()
	{
		std::string buffer;
		while (!service.stopped())
		{
			std::getline(std::cin, buffer);
			if (buffer == "users")
			{
				asio::post(peer_strand, [this]() {
					std::cout << "connected peers : " << connected_peers.size() << '\n';
					for (const auto& p : connected_peers)
					{
						std::cout << p.first << std::endl;
					}

					std::cout << "not connected peers : " << known_peers.size() << '\n';
					for (const auto& p : known_peers)
					{
						std::cout << p.first << std::endl;
					}
					});
			}
			else
			{
				auto pos = buffer.find('|');
				if (pos != std::string::npos)
				{
					asio::post(peer_strand, [this, username = buffer.substr(0, pos), message = buffer.substr(pos + 1)] () mutable
					{
						if (message.size() > 512)
						{
							std::cout << "Warning: message limit 512 character !\n";
							message.resize(512);
						}
						peer_lookout_and_send_message(username, message);
					});
				}
			}
		}
	}

	void main::connect_to_server()
	{
		asio::ip::tcp::resolver resolver(service);
		asio::error_code ec;
		auto eps = resolver.resolve(conf.server_ip, conf.server_port, ec);

		asio::async_connect(server_sock, eps,
			[this, eps](asio::error_code ec, asio::ip::tcp::endpoint ep) {
				if (ec)
				{
					//std::cout
					//	<< "Failed to connect to server: " << ep.address().to_string() << ':' << ep.port() << '\n'
					//	<< "You may want to check if the configuration file matches\n"
					//	<< "retry in 30s" << std::endl;
					retry_timer.expires_after(std::chrono::seconds(conf.second_before_server_retry));
					retry_timer.async_wait([this](asio::error_code) { connect_to_server(); });
					return;
				}

				common::message login = config::log_me_from_config(conf);
				std::string buffer;
				common::message::serialize(login, buffer);
				asio::write(server_sock, asio::buffer(buffer), ec);
				if (!ec)
				{
					setup_server_read();
				}
			});
	}

	void main::run()
	{
		std::vector<std::thread> thds;

		auto thread_num = std::min(conf.thread_number, max_thread);

		for (int i = 0; i < thread_num - 1; i++)
		{
			thds.emplace_back([this]() {
				service.run();
				});
		}
		thds.emplace_back([this]() {
			service.run();
			});

		asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), conf.listening_port);
		incoming_connection.open(ep.protocol());
		incoming_connection.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		incoming_connection.bind(ep);
		incoming_connection.listen();

		std::cout << "start listening on port: " << conf.listening_port << '\n';
		start_accept();
		connect_to_server();
		handle_commandline();

		for (auto& t : thds)
		{
			t.join();
		}
	}
}
