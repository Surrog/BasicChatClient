
#include "network.hpp"

namespace client
{
	network::network()
		: service(), signals(service, SIGINT, SIGTERM), peers(), sock(service)
	{
		signals.async_wait([this](auto, auto) {
			service.stop();
			});


	}
}