#pragma once
#include "http_server_context.hpp"

namespace kumori
{

	class regex_content : boost::noncopyable
	{
	public:

		virtual ~regex_content()
		{
		}

		virtual const std::regex& pattern() const = 0;

		virtual void on_get(http_server_context& /*context*/, bool /*head*/, const std::smatch& /*match*/)
		{
			BOOST_THROW_EXCEPTION(http_exception(http_status_code::method_not_allowed));
		}

		virtual void on_post(http_server_context& /*context*/, const std::smatch& /*match*/)
		{
			BOOST_THROW_EXCEPTION(http_exception(http_status_code::method_not_allowed));
		}

	};

}
