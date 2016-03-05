#pragma once
#include "database_connection.hpp"
#include "redis_database_config.hpp"
#include "redis_client.hpp"

namespace kumori
{

	class redis_database_connection : public database_connection
	{
	public:

		redis_database_connection(
			boost::asio::io_service& service,
			std::string host,
			unsigned short port,
			const redis_database_config& config)
			: client_(service, std::move(host), port, config.client)
		{
		}

		virtual void ping() override
		{
			client_.ping();
		}

		virtual boost::optional<std::string> get(const std::string& key) override
		{
			return client_.get(key);
		}

		virtual void set(const std::string& key, const std::string& value) override
		{
			client_.set(key, value);
		}

	private:

		redis_client client_;

	};

}
