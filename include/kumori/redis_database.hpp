#pragma once
#include "database.hpp"
#include "redis_database_connection.hpp"
#include "redis_database_config.hpp"
#include "interrupted_exception.hpp"
#include "async_mutex.hpp"
#include "sync_lock.hpp"

namespace kumori
{

	class redis_database : public database
	{
	public:

		redis_database(
			boost::asio::io_service& service,
			const std::string& host,
			unsigned short port,
			const redis_database_config& config = redis_database_config())
			: service_(service)
			, host_(host)
			, port_(port)
			, config_(config)
			, mutex_(service)
		{
		}

		virtual void connect(const std::function<void(database_connection&)>& callback) override
		{
			sync_lock lock(mutex_);

			if (connection_)
			{
				try
				{
					connection_->ping();
				}
				catch (interrupted_exception&)
				{
					throw;
				}
				catch (boost::context::detail::forced_unwind&)
				{
					throw;
				}
				catch (...)
				{
					connection_.reset();
				}
			}

			if (!connection_)
				connection_ = std::make_unique<redis_database_connection>(service_, host_, port_, config_);

			callback(*connection_);
		}

	private:

		boost::asio::io_service& service_;
		std::string host_;
		unsigned short port_;
		redis_database_config config_;

		std::unique_ptr<redis_database_connection> connection_;
		async_mutex mutex_;

	};

}
