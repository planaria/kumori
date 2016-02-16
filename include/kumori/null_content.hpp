#pragma once
#include "http_context.hpp"

namespace kumori
{

	class null_content
	{
	public:

		void on_get(http_server_context& context, bool head)
		{
			context.response().set_content_length(0);
			context.write_headers();
		}

	};

}
