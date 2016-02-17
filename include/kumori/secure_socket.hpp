#pragma once
#include "socket_adaptor.hpp"

namespace kumori
{

	class secure_socket : public socket_adaptor<secure_socket>
	{
	public:

		secure_socket(boost::asio::io_service& service, boost::asio::ssl::context& context)
			: socket_(service)
			, stream_(socket_, context)
		{
		}

		secure_socket(boost::asio::ip::tcp::socket&& socket, boost::asio::ssl::context& context)
			: socket_(std::move(socket))
			, stream_(socket_, context)
		{
		}

		~secure_socket()
		{
			boost::system::error_code err;
			socket_.close(err);
		}

		virtual boost::asio::ip::tcp::socket& raw_socket() override
		{
			return socket_;
		}

		virtual void handshake(bool server, const boost::posix_time::time_duration& timeout) override
		{
			sync& sync = sync::current();

			boost::asio::ssl::stream_base::handshake_type type = server
				? boost::asio::ssl::stream_base::server
				: boost::asio::ssl::stream_base::client;

			boost::system::error_code error;

			stream_.async_handshake(type,
				[&](const boost::system::error_code& e)
			{
				error = e;
				sync.resume();
			});

			sync.suspend_for(timeout,
				[&]()
			{
				socket_.cancel();
			});

			if(error == boost::asio::error::operation_aborted)
				BOOST_THROW_EXCEPTION(interrupted_exception());
			else if(error)
				BOOST_THROW_EXCEPTION(boost::system::system_error(error));
		}

		template <class Buffer, class Callback>
		void async_read_some(const Buffer& buffer, Callback callback)
		{
			stream_.async_read_some(buffer, callback);
		}

		template <class Buffer, class Callback>
		void async_write_some(const Buffer& buffer, Callback callback)
		{
			stream_.async_write_some(buffer, callback);
		}

	private:

		boost::asio::ip::tcp::socket socket_;
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> stream_;

	};

}
