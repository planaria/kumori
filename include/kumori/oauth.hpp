#pragma once
#include "http_server_context.hpp"

namespace kumori
{

	class oauth : boost::noncopyable
	{
	public:

		virtual ~oauth()
		{
		}

		virtual void on_get(http_server_context& context) = 0;

		virtual void on_get_callback(http_server_context& context) = 0;

	};

}
