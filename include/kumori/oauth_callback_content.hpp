#pragma once
#include "content.hpp"
#include "oauth.hpp"

namespace kumori
{

	class oauth_callback_content : public content
	{
	public:

		oauth_callback_content(oauth& oauth, std::string location)
			: oauth_(oauth)
			, location_(std::move(location))
		{
		}

		virtual void on_get(http_server_context& context, bool /*head*/) override
		{
			oauth_.on_get_callback(context);

			http_response& res = context.response();
			res.set_status_code(http_status_code::found);
			res.set_location(location_);
			res.set_content_length(0);

			context.write_headers();
		}

	private:

		oauth& oauth_;

		std::string location_;

	};

}
