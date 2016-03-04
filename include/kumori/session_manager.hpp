#pragma once
#include "session.hpp"
#include "user_info.hpp"
#include "database.hpp"
#include "http_server_context.hpp"
#include "secure_random.hpp"
#include "csrf_exception.hpp"

namespace kumori
{

	class session_manager
	{
	public:

		session_manager(database& database, http_server_context& context)
			: database_(database)
			, context_(context)
		{
			http_request& req = context.request();
			http_response& res = context.response();

			auto now = boost::posix_time::second_clock::universal_time();

			if (req.cookie().get("SID", session_id_))
			{
				bool found = false;

				database.connect([&](database_connection& con)
				{
					auto s = kumori::get<session>(con, "session", session_id_);

					if (s)
					{
						session_ = std::move(*s);
						session_.timestamp = now;
						con.set("session", session_id_, session_);
						found = true;
					}
				});

				if (found)
					return;
			}

			secure_random& random = secure_random::get();

			session_id_ = random.uuid();
			session_.timestamp = now;
			session_.csrf_token = random.uuid();

			database.connect([&](database_connection& con)
			{
				con.set("session", session_id_, session_);
			});

			res.cookie().set("SID", session_id_);
		}

		const session& get() const
		{
			return session_;
		}

		void verify_csrf_token()
		{
			http_request& req = context_.request();
			if (req.get_csrf_token() != boost::lexical_cast<std::string>(session_.csrf_token))
				BOOST_THROW_EXCEPTION(csrf_exception());
		}

		void begin_oauth(const std::string& site_name, const std::string& token, const std::string& token_secret)
		{
			session_.oauth_site_name = site_name;
			session_.oauth_token = token;
			session_.oauth_token_secret = token_secret;

			database_.connect([&](database_connection& con)
			{
				con.set("session", session_id_, session_);
			});
		}

		bool verify_oauth_token(const std::string& site_name, const std::string& token)
		{
			if (session_.oauth_site_name != site_name)
				return false;
			if (session_.oauth_token != token)
				return false;

			session_.oauth_site_name.clear();
			session_.oauth_token.clear();
			session_.oauth_token_secret.clear();

			database_.connect([&](database_connection& con)
			{
				con.set("session", session_id_, session_);
			});

			return true;
		}

		void sign_in(const std::string& site_name, const std::string& site_user_id, const user_info& user_info)
		{
			database_.connect([&](database_connection& con)
			{
				auto user_id = kumori::get<boost::uuids::uuid>(con, "user-id", site_name, site_user_id);

				if (!user_id)
				{
					secure_random& random = secure_random::get();
					user_id = random.uuid();
					con.set("user-id", site_name, site_user_id, user_id);
				}

				session_.user_id = *user_id;
				con.set("session", session_id_, session_);

				con.set("user-info", *user_id, user_info);
			});
		}

		void sign_out()
		{
			if (!session_.user_id)
				return;

			session_.user_id = boost::none;

			database_.connect([&](database_connection& con)
			{
				con.set("session", session_id_, session_);
			});
		}

	private:

		database& database_;
		http_server_context& context_;

		boost::uuids::uuid session_id_;
		session session_;

	};

}
