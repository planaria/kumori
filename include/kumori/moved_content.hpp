#pragma once
#include "content.hpp"

namespace kumori
{

	class moved_content : public content
	{
	public:

		explicit moved_content(std::string location)
			: location_(std::move(location))
		{
		}

		virtual void on_get(http_server_context& context, bool /*head*/) override
		{
			http_response& res = context.response();
			res.set_status_code(http_status_code::moved_permanently);
			res.set_location(location_);
			res.set_content_length(0);
			context.write_headers();
		}

		virtual void on_post(http_server_context& context) override
		{
			on_get(context, false);
		}

	private:

		std::string location_;

	};

}
