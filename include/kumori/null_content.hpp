#pragma once
#include "content.hpp"

namespace kumori
{

	class null_content : public content
	{
	public:

		virtual void on_get(http_server_context& context, bool /*head*/) override
		{
			context.response().set_content_length(0);
			context.write_headers();
		}

	};

}
