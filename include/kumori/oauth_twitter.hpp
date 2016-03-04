#pragma once
#include "oauth_10.hpp"
#include "oauth_twitter_config.hpp"
#include "picojson_exception.hpp"
#include "picojson_util.hpp"

namespace kumori
{

	class oauth_twitter : public oauth_10
	{
	public:

		oauth_twitter(boost::asio::io_service& service, database& database, const oauth_twitter_config& config)
			: oauth_10(service, database, config)
			, verify_credentials_url_(config.verify_credentials_url)
		{
		}

	protected:

		using typename oauth_10::parameters_type;

		virtual std::pair<std::string, user_info> get_user_id_info(
			const std::string& access_token,
			const std::string& access_token_secret) override
		{
			parameters_type parameters;

			parameters["include_entities"] = "false";
			parameters["skip_status"] = "true";

			std::string response_string = this->signed_request(
				http_method::get, verify_credentials_url_, parameters,
				access_token, access_token_secret);

			picojson::value value;
			CHECK_PICOJSON(parse(value, response_string.begin(), response_string.end()));
			if (!value.is<picojson::object>())
				BOOST_THROW_EXCEPTION(oauth_exception());
			const picojson::object& tree = value.get<picojson::object>();

			std::string id = get_or_empty(tree, "id_str");
			if(id.empty())
				BOOST_THROW_EXCEPTION(oauth_exception());

			user_info info;
			info.name = get_or_empty(tree, "name");
			if (info.name.empty())
				BOOST_THROW_EXCEPTION(oauth_exception());

			info.icon = get_or_empty(tree, "profile_image_url");

			return std::make_pair(std::move(id), std::move(info));
		}

	private:

		url verify_credentials_url_;

	};

}
