#pragma once
#include "content.hpp"
#include "oauth.hpp"

namespace kumori
{

	class oauth_content : public content
	{
	public:

		explicit oauth_content(oauth& oauth)
			: oauth_(oauth)
		{
		}

		virtual void on_get(http_server_context& context, bool /*head*/) override
		{
			oauth_.on_get(context);

			context.response().set_content_length(0);
			context.write_headers();
		}

	private:

		oauth& oauth_;

	};

}
