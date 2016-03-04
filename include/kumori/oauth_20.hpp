#pragma once
#include "oauth.hpp"
#include "oauth_20_config.hpp"
#include "database.hpp"
#include "http_client.hpp"
#include "session_manager.hpp"
#include "stream_util.hpp"
#include "oauth_exception.hpp"
#include "picojson_exception.hpp"
#include "picojson_util.hpp"

namespace kumori
{

	class oauth_20 : public oauth
	{
	public:

		oauth_20(boost::asio::io_service& service, database& database, const oauth_20_config& config)
			: service_(service)
			, database_(database)
			, config_(config)
		{
			if (!config.certificate.empty())
			{
				ssl_context_ = std::make_unique<boost::asio::ssl::context>(service, boost::asio::ssl::context::sslv23);
				ssl_context_->add_certificate_authority(boost::asio::const_buffer(config.certificate.data(), config.certificate.size()));
			}

			encoded_callback_url_ = encode_url(config.callback_url.get_raw());
			encoded_client_id_ = encode_url(config.client_id);
			encoded_client_secret_ = encode_url(config.client_secret);

			authorize_url_ = config.authorize_url;
			access_token_url_ = config.access_token_url;
		}

		virtual void on_get(http_server_context& context) override
		{
			std::ostringstream location;
			location
				<< authorize_url_.get_raw()
				<< "?response_type=code"
				<< "&client_id=" << encoded_client_id_
				<< "&redirect_uri=" << encoded_callback_url_
				<< "&scope=profile";

			http_response& res = context.response();
			res.set_status_code(http_status_code::found);
			res.set_location(location.str());
		}

		virtual void on_get_callback(http_server_context& context) override
		{
			http_request& req = context.request();
			const auto& params = req.parameters();

			auto error_it = params.find("error");
			if(error_it != params.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto code_it = params.find("code");
			if(code_it == params.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			std::stringstream parameters;
			parameters
				<< "code=" << encode_url(code_it->second)
				<< "&client_id=" << encoded_client_id_
				<< "&client_secret=" << encoded_client_secret_
				<< "&redirect_uri=" << encoded_callback_url_
				<< "&grant_type=authorization_code";

			std::string response_str = send(parameters);

			picojson::value value;
			CHECK_PICOJSON(parse(value, response_str.begin(), response_str.end()));
			if(!value.is<picojson::object>())
				BOOST_THROW_EXCEPTION(oauth_exception());
			const picojson::object& tree = value.get<picojson::object>();

			std::string access_token = get_or_empty(tree, "access_token");
			if(access_token.empty())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto user_id_info = get_user_id_info(access_token);

			session_manager manager(database_, context);
			manager.sign_in(config_.site_name, user_id_info.first, user_id_info.second);
		}

	protected:

		virtual std::pair<std::string, user_info> get_user_id_info(
			const std::string& access_token) = 0;

	private:

		std::string send(std::stringstream& parameters)
		{
			http_client_config config;
			config.ssl_context = ssl_context_.get();

			http_client client(service_, access_token_url_, config);

			http_request& req = client.request();
			req.set_method(http_method::post);
			req.set_content_type("application/x-www-form-urlencoded");
			req.set_content_length(get_size(parameters));

			client.write_headers();

			forward(parameters, client.request_stream());

			client.finish_write();
			client.read_headers();

			std::string response_str = read_until_end(client.response_stream());

			client.finish_read();

			http_status_code status_code = client.response().get_status_code();
			if (status_code != http_status_code::ok)
				BOOST_THROW_EXCEPTION(oauth_exception());

			return response_str;
		}

		boost::asio::io_service& service_;
		database& database_;
		oauth_20_config config_;

		std::string encoded_callback_url_;
		std::string encoded_client_id_;
		std::string encoded_client_secret_;

		url authorize_url_;
		url access_token_url_;

		std::unique_ptr<boost::asio::ssl::context> ssl_context_;

	};

}
