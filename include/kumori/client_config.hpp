#pragma once

namespace kumori
{

	struct client_config
	{

		std::size_t socket_buffer_size = 1024;

		bool enable_ssl = false;
		boost::asio::ssl::context* ssl_context = nullptr;

		boost::posix_time::time_duration socket_timeout = boost::posix_time::seconds(10);

	};

}
