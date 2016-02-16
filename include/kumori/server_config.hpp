#pragma once

namespace kumori
{

	struct server_config
	{

		bool enable_plain = true;
		int plain_port = 80;

		bool enable_ssl = false;
		int ssl_port = 443;
		boost::filesystem::path certificate_chain_file;
		boost::filesystem::path private_key_file;
		boost::filesystem::path tmp_dh_file;

		std::size_t num_maximum_connections = 10000;
		std::size_t stack_size_per_connection = 102400;
		std::size_t socket_buffer_size = 1024;
		boost::posix_time::time_duration socket_timeout = boost::posix_time::seconds(10);

	};

}
