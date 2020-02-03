
#include <iostream>
#include "main.hpp"
#include "asio.hpp"

namespace client
{
	main::main(config conf)
		: service(), signals(service, SIGINT, SIGTERM)
		, server_sock(service), new_connection(service)
		, incoming_connection(service), conf(conf)
		, connected_peers(), known_peers()
		, peer_strand(service)

	{
		signals.async_wait([this](auto, auto) {
			service.stop();
			});
	}

	void main::start_accept()
	{
		incoming_connection.async_accept(new_connection, [this](asio::error_code ec) {
			auto new_peer = std::make_shared<peer>(*this, std::move(new_connection));

			new_peer->setup_read();

			});
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

		common::message login;
		login.id = common::message::id_t::LOG_ME;
		login.data = conf.username;

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

	void main::register_connected_client(const std::shared_ptr<peer>& cli, bool was_known)
	{
		asio::post(peer_strand, [this, cli, was_known] {
			if (was_known)
			{
				known_peers.erase(cli->username());
			}
			connected_peers[cli->username()] = cli;
			});
	}

	void main::cleanup_failed_client(const std::shared_ptr<peer>& cli)
	{
		asio::post(peer_strand, [this, cli] {
			known_peers.erase(cli->username());
			connected_peers.erase(cli->username());
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

		std::string buffer;
		while (!service.stopped())
		{
			std::cin >> buffer;
			if (buffer == "users")
			{
				asio::post(peer_strand, [this]() {
					std::cout << "connected peers : " << connected_peers.size() << '\n';
					for (const auto& p : connected_peers)
					{
						std::cout << p.first << std::endl;
					}

					std::cout << "known peers : " << known_peers.size() << '\n' << std::endl;
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
					asio::post(peer_strand, [this, username = buffer.substr(0, pos), message = buffer.substr(pos + 1)]()
					{
						peer_lookout_and_send_message(username, message);
					});
				}
			}
		}

		for (auto& t : thds)
		{
			t.join();
		}
	}
}
