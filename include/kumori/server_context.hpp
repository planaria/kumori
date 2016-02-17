#pragma once
#include "server_config.hpp"
#include "socket.hpp"
#include "sync.hpp"
#include "socket_streambuf.hpp"

namespace kumori
{

	template <class Server>
	class server_context : boost::noncopyable
	{
	public:

		explicit server_context(
			boost::asio::io_service& service,
			Server& server,
			const server_config& config)
			: service_(service)
			, server_(server)
			, config_(config)
			, sync_(service_, std::bind(&server_context::run, this), config.stack_size_per_connection)
			, input_buffer_(config.socket_buffer_size)
			, output_buffer_(config.socket_buffer_size)
		{
			sync_.resume();
		}

		void start(std::unique_ptr<socket> socket)
		{
			socket_ = std::move(socket);
			sync_.resume();
		}

		void interrupt()
		{
			sync_.interrupt();
		}

	private:

		void run()
		{
			while (true)
			{
				server_.on_ready(this);
				sync_.suspend();

				try
				{
					socket_->handshake(true, config_.socket_timeout);

					socket_streambuf sb(
						*socket_,
						config_.socket_timeout,
						input_buffer_.data(), input_buffer_.size(),
						output_buffer_.data(), output_buffer_.size());

					std::iostream stream(&sb);
					stream.exceptions(std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);

					server_.derived().on_connected(stream);
				}
				catch (...)
				{
				}

				socket_.reset();
			}
		}

		boost::asio::io_service& service_;
		Server& server_;
		server_config config_;

		sync sync_;

		std::unique_ptr<socket> socket_;

		std::vector<char> input_buffer_;
		std::vector<char> output_buffer_;

	};

}
