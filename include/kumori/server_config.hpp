#pragma once
#include "logger.hpp"

namespace kumori
{

	struct server_config
	{

		bool enable_plain = true;
		unsigned short plain_port = 80;

		bool enable_ssl = false;
		unsigned short ssl_port = 443;
		boost::asio::ssl::context* ssl_context = nullptr;

		std::size_t num_maximum_connections = 10000;
		std::size_t stack_size_per_connection = 102400;
		std::size_t socket_buffer_size = 1024;
		boost::posix_time::time_duration socket_timeout = boost::posix_time::seconds(10);

		logger* logger = nullptr;

	};

}
