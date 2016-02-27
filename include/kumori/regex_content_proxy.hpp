#pragma once
#include "content.hpp"
#include "regex_content.hpp"

namespace kumori
{

	class regex_content_proxy : public content
	{
	public:

		regex_content_proxy(regex_content& content, const std::smatch& match)
			: content_(content)
			, match_(match)
		{
		}

		virtual void on_get(http_server_context& context, bool head) override
		{
			content_.on_get(context, head, match_);
		}

		virtual void on_post(http_server_context& context) override
		{
			content_.on_post(context, match_);
		}

	private:

		regex_content& content_;
		const std::smatch& match_;

	};

}
