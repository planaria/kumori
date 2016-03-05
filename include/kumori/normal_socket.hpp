#pragma once
#include "socket_adaptor.hpp"

namespace kumori
{

	class normal_socket : public socket_adaptor<normal_socket>
	{
	public:

		explicit normal_socket(boost::asio::io_service& service)
			: socket_(service)
		{
		}

		explicit normal_socket(boost::asio::ip::tcp::socket&& socket)
			: socket_(std::move(socket))
		{
		}

		~normal_socket()
		{
			boost::system::error_code err;
			socket_.close(err);
		}

		virtual boost::asio::ip::tcp::socket& raw_socket() override
		{
			return socket_;
		}

		virtual void handshake(bool /*server*/, const boost::posix_time::time_duration& /*timeout*/) override
		{
		}

		template <class Buffer, class Callback>
		void async_read_some(const Buffer& buffer, Callback& callback)
		{
			socket_.async_read_some(buffer, callback);
		}

		template <class Buffer, class Callback>
		void async_write_some(const Buffer& buffer, Callback& callback)
		{
			socket_.async_write_some(buffer, callback);
		}

	private:

		boost::asio::ip::tcp::socket socket_;

	};

}
