
#include <string>
#include <iostream>
#include <unordered_map>
#include "message.hpp"
#include "peer.hpp"


int main(int argc, char** argv)
{
	std::cout << R"_(usage: `user:"message"`\n)_";
	common::message m;
	m.id = common::message::id_t::LOG_ME;
	m.data = "toto";
	std::string buff;
	common::message::serialize(m, buff);
	std::cout << buff << '\n';

	asio::io_context network_service;
	std::thread r([&network_service]() {
		network_service.run_one_until(std::chrono::system_clock::now() + std::chrono::seconds(10));
		});

	asio::ip::tcp::resolver resol(network_service);
	asio::error_code ec;
	auto ep = resol.resolve("localhost", "8752");
	asio::ip::tcp::socket sock(network_service);
	asio::connect(sock, ep, ec);
	if (!ec)
	{
		asio::write(sock, asio::buffer(buff), ec);
		if (ec)
		{
			std::cout << "write error " << ec << '\n';
		}
	}
	else
	{
		std::cout << "connect error " << ec << '\n';
	}

	r.join();
}
