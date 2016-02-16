#pragma once

namespace kumori
{

	class socket : boost::noncopyable
	{
	public:

		virtual ~socket()
		{
		}

		virtual boost::asio::ip::tcp::socket& raw_socket() = 0;

		virtual void handshake(bool server, const boost::posix_time::time_duration& timeout) = 0;

		virtual std::size_t read_some(char* buffer, std::size_t buffer_size, const boost::posix_time::time_duration& timeout) = 0;

		virtual std::size_t write_some(const char* buffer, std::size_t buffer_size, const boost::posix_time::time_duration& timeout) = 0;

	};

}
