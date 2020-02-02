
#include "tracker.hpp"
#include <thread>

namespace server
{
	tracker::tracker(const tracker_config& config)
		: config(config), network_service(), signals(network_service, SIGINT, SIGTERM), acceptor(network_service), sock(network_service), connection_strand(network_service)
	{
		signals.async_wait([this](auto, auto) {
			network_service.stop();
			});

		asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), config.port);
		acceptor.open(ep.protocol());
		acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		acceptor.bind(ep);
		acceptor.listen();

		start_accept();
	}

	void tracker::run(std::size_t thread_num)
	{
		thread_num = std::min(max_thread, thread_num);
		std::vector<std::thread> threads;

		if (thread_num > 1)
		{
			threads.emplace_back([this]() { network_service.run(); });
		}
		network_service.run();

		for (auto& thr : threads)
		{
			thr.join();
		}
	}

	void tracker::start_accept()
	{
		acceptor.async_accept(sock, [this](asio::error_code ec)
			{
				if (!ec)
				{
					setup_client(std::move(sock));
				}
				start_accept();
			});
	}

	void tracker::remove_bad_client(const std::shared_ptr<client>& cli)
	{
		if (cli->status != client::status_t::errored)
		{
			cli->status = client::status_t::errored;
			asio::post(connection_strand, [this, cli]() {
				auto it = std::find(current_connection.begin(), current_connection.end(), cli);
				if (it != current_connection.end())
				{
					current_connection.erase(it);
				}
				});
		}
	}

	void tracker::setup_client(asio::ip::tcp::socket s)
	{
		auto new_client = std::make_shared<client>(std::move(s), *this, network_service);

		new_client->setup_handle();
		asio::post(connection_strand, [this, new_client](){
			current_connection.push_back(std::move(new_client));
		});
	}

	void tracker::broadcast_message(const common::message& mess)
	{
		std::string buffer;
		common::message::serialize(mess, buffer);

		for (auto& cli : current_connection)
		{
			cli->write_buffer(buffer);
		}
	}
}