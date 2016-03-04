#pragma once
#include "database.hpp"
#include "redis_database_connection.hpp"
#include "redis_database_config.hpp"
#include "interrupted_exception.hpp"

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
		{
		}

		virtual void connect(const std::function<void(database_connection&)>& callback) override
		{
			redis_database_connection connection(service_, host_, port_, config_);
			callback(connection);
		}

	private:

		boost::asio::io_service& service_;
		std::string host_;
		unsigned short port_;
		redis_database_config config_;

	};

}
