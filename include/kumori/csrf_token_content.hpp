#pragma once
#include "content.hpp"
#include "session_manager.hpp"

namespace kumori
{

	class csrf_token_content : public content
	{
	public:

		explicit csrf_token_content(database& database, const session_manager_config& config = session_manager_config())
			: database_(database)
			, config_(config)
		{
		}

		virtual void on_post(http_server_context& context) override
		{
			session_manager manager(database_, context, config_);
			auto& session = manager.get();

			context.response().set_mime_type("application/json; charset=utf-8");
			context.write_headers();

			picojson::object obj;
			obj["csrf_token"] = picojson::value(boost::lexical_cast<std::string>(session.csrf_token));

			std::ostreambuf_iterator<char> response_it(context.response_stream());
			picojson::value(obj).serialize(response_it);
		}

	private:

		database& database_;
		session_manager_config config_;

	};

}
