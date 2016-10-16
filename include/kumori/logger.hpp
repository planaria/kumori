#pragma once
#include "http_request.hpp"

namespace kumori
{

	class logger : boost::noncopyable
	{
	public:

		virtual ~logger()
		{
		}

		virtual void server_starting() = 0;

		virtual void server_started() = 0;

		virtual void server_stopping() = 0;

		virtual void server_stopped() = 0;

		virtual void server_listen(bool secure, unsigned short port) = 0;

		virtual void server_accept(const boost::asio::ip::tcp::endpoint& endpoint) = 0;

		virtual void server_disconnect(const boost::asio::ip::tcp::endpoint& endpoint) = 0;

		virtual void http_server_request(const http_request& req) = 0;

		virtual void exception() = 0;

	};

}
