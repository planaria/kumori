#pragma once
#include "client_config.hpp"
#include "normal_socket.hpp"
#include "secure_socket.hpp"
#include "socket_streambuf.hpp"
#include "sync.hpp"
#include "interrupted_exception.hpp"

namespace kumori
{

	class client : boost::noncopyable
	{
	public:

		explicit client(
			boost::asio::io_service& service,
			const std::string& host,
			const std::string& port,
			const client_config& config = client_config())
			: socket_(connect(service, host, port, config))
			, input_buffer_(config.socket_buffer_size)
			, output_buffer_(config.socket_buffer_size)
			, streambuf_(*socket_, config.socket_timeout, input_buffer_, output_buffer_)
			, stream_(&streambuf_)
		{
		}

		std::iostream& stream()
		{
			return stream_;
		}

	private:

		std::unique_ptr<socket> connect(boost::asio::io_service& service, const std::string& host, const std::string& port, const client_config& config)
		{
			sync& sync = sync::current();

			std::unique_ptr<socket> socket;

			if (config.enable_ssl)
				socket = std::make_unique<secure_socket>(service, *config.ssl_context);
			else
				socket = std::make_unique<normal_socket>(service);

			boost::asio::ip::tcp::resolver resolver(service);
			boost::asio::ip::tcp::resolver::query query(host, port);

			boost::system::error_code error;
			boost::asio::ip::tcp::resolver::iterator it;

			resolver.async_resolve(query,
				[&](const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator it_)
			{
				error = e;
				it = it_;
				sync.resume();
			});

			sync.suspend_for(config.socket_timeout,
				[&]()
			{
				resolver.cancel();
			});

			if (error == boost::asio::error::operation_aborted)
				BOOST_THROW_EXCEPTION(interrupted_exception());
			else if (error)
				BOOST_THROW_EXCEPTION(boost::system::system_error(error));

			boost::asio::async_connect(socket->raw_socket(), it,
				[&](const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator)
			{
				error = e;
				sync.resume();
			});

			sync.suspend_for(config.socket_timeout,
				[&]()
			{
				socket->raw_socket().cancel();
			});

			if (error == boost::asio::error::operation_aborted)
				BOOST_THROW_EXCEPTION(interrupted_exception());
			else if (error)
				BOOST_THROW_EXCEPTION(boost::system::system_error(error));

			socket->handshake(false, config.socket_timeout);

			return socket;
		}

		std::unique_ptr<socket> socket_;
		std::vector<char> input_buffer_;
		std::vector<char> output_buffer_;
		socket_streambuf streambuf_;
		std::iostream stream_;

	};

}
