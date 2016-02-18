#pragma once
#include "server_config.hpp"
#include "server_context.hpp"
#include "normal_socket.hpp"
#include "secure_socket.hpp"
#include "invalid_operation_exception.hpp"

namespace kumori
{

	template <class Derived>
	class server : boost::noncopyable
	{
	public:

		explicit server(boost::asio::io_service& service, const server_config& config = server_config())
			: service_(service)
			, config_(config)
		{
		}

		~server()
		{
			if (running_)
				stop();
		}

		bool is_running() const
		{
			return running_;
		}

		void start()
		{
			if (running_)
				BOOST_THROW_EXCEPTION(invalid_operation_exception());

			contexts_.resize(config_.num_maximum_connections);
			context_stack_ = std::make_unique<boost::lockfree::stack<context*>>(config_.num_maximum_connections);

			if (config_.enable_plain)
			{
				boost::asio::ip::tcp::endpoint plain_end_point(boost::asio::ip::tcp::v6(), config_.plain_port);
				plain_accceptor_ = std::make_unique<acceptor>(service_, plain_end_point);
			}

			if (config_.enable_ssl)
			{
				boost::asio::ip::tcp::endpoint ssl_end_point(boost::asio::ip::tcp::v6(), config_.ssl_port);
				ssl_accceptor_ = std::make_unique<acceptor>(service_, ssl_end_point);
			}

			std::generate(contexts_.begin(), contexts_.end(),
				[&]()
			{
				return std::make_unique<context>(service_, *this, config_);
			});

			if (config_.enable_plain)
				accept(false);

			if (config_.enable_ssl)
				accept(true);

			running_ = true;
		}

		void stop()
		{
			if (!running_)
				BOOST_THROW_EXCEPTION(invalid_operation_exception());

			std::size_t count_stopped = 0;

			if (plain_accceptor_)
				plain_accceptor_->close();

			if (ssl_accceptor_)
				ssl_accceptor_->close();

			while (count_stopped != config_.num_maximum_connections)
			{
				while (!context_stack_->empty())
				{
					context* context;
					if (context_stack_->pop(context))
						++count_stopped;
				}

				for (auto& context : contexts_)
					context->interrupt();
			}

			plain_accceptor_.reset();
			ssl_accceptor_.reset();

			contexts_.clear();
			context_stack_.reset();

			running_ = false;
		}

	private:

		friend class server_context<server>;

		typedef server_context<server> context;

		Derived& derived()
		{
			return static_cast<Derived&>(*this);
		}

		void accept(bool secure)
		{
			acceptor& acc = secure ? *ssl_accceptor_ : *plain_accceptor_;
			auto raw_socket = std::make_shared<boost::asio::ip::tcp::socket>(service_);

			acc.async_accept(*raw_socket, [=](const boost::system::error_code& error)
			{
				if (error == boost::asio::error::operation_aborted)
					return;

				accept(secure);

				context* context;

				while (!context_stack_->pop(context))
				{
					if (context_stack_->empty())
						return;
				}

				std::unique_ptr<socket> socket;
				if (secure)
					socket = std::make_unique<secure_socket>(std::move(*raw_socket), *config_.ssl_context);
				else
					socket = std::make_unique<normal_socket>(std::move(*raw_socket));

				context->start(std::move(socket));
			});
		}

		void on_ready(context* context)
		{
			while (!context_stack_->push(context)) {}
		}

		boost::asio::io_service& service_;
		server_config config_;

		typedef boost::asio::ip::tcp::acceptor acceptor;

		std::unique_ptr<acceptor> plain_accceptor_;
		std::unique_ptr<acceptor> ssl_accceptor_;

		std::vector<std::unique_ptr<context>> contexts_;
		std::unique_ptr<boost::lockfree::stack<context*>> context_stack_;

		bool running_ = false;

	};

}
