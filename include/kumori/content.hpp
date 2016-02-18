#pragma once
#include "http_server_context.hpp"

namespace kumori
{

	class content : boost::noncopyable
	{
	public:

		virtual void on_get(http_server_context& context, bool head)
		{
			BOOST_THROW_EXCEPTION(http_exception(http_status_code::method_not_allowed));
		}

		virtual void on_post(http_server_context& context)
		{
			BOOST_THROW_EXCEPTION(http_exception(http_status_code::method_not_allowed));
		}

	};

}
