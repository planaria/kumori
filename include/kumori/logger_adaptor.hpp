#pragma once
#include "logger.hpp"

namespace kumori
{

	class logger_adaptor : public logger
	{
	public:

		virtual void server_starting() override
		{
			info("server starting");
		}

		virtual void server_started() override
		{
			info("server started");
		}

		virtual void server_stopping() override
		{
			info("server stopping");
		}

		virtual void server_stopped() override
		{
			info("server stopped");
		}

		virtual void server_listen(bool secure, unsigned short port) override
		{
			info(boost::str(
				boost::format("listen %1% port %2%")
				% (secure ? "secure" : "plain")
				% port
			));
		}

		virtual void server_accept(const boost::asio::ip::tcp::endpoint& endpoint) override
		{
			info(boost::str(
				boost::format("accept %1%")
				% endpoint
			));
		}

		virtual void server_disconnect(const boost::asio::ip::tcp::endpoint& endpoint) override
		{
			info(boost::str(
				boost::format("disconnect %1%")
				% endpoint
			));
		}

		virtual void http_server_request(const http_request& req) override
		{
			info(boost::str(
				boost::format("request %1% %2%")
				% req.get_method()
				% req.get_path()
			));
		}

		virtual void exception() override
		{
			try
			{
				throw;
			}
			catch (std::exception& e)
			{
				error(e.what());
			}
			catch (...)
			{
				error("unknown error");
			}
		};

		virtual void info(const std::string& message) = 0;

		virtual void error(const std::string& message) = 0;

	};

}
