#pragma once
#include "oauth.hpp"
#include "oauth_10_config.hpp"
#include "database.hpp"
#include "http_client.hpp"
#include "session_manager.hpp"
#include "secure_random.hpp"
#include "base64_util.hpp"
#include "hash_util.hpp"
#include "stream_util.hpp"
#include "oauth_exception.hpp"

namespace kumori
{

	class oauth_10 : public oauth
	{
	public:

		oauth_10(boost::asio::io_service& service, database& database, const oauth_10_config& config)
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
			encoded_consumer_key_ = encode_url(config.consumer_key);
			encoded_consumer_secret_ = encode_url(config.consumer_secret);

			request_token_url_ = config.request_token_url;
			authorize_url_ = config.authorize_url;
			access_token_url_ = config.access_token_url;
		}

		virtual void on_get(http_server_context& context) override
		{
			parameters_type parameters;
			parameters["oauth_callback"] = encoded_callback_url_;

			tokens_type tokens = parse_tokens(signed_request(http_method::post, request_token_url_, parameters));

			auto token_it = tokens.find("oauth_token");
			if (token_it == tokens.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto token_secret_it = tokens.find("oauth_token_secret");
			if (token_secret_it == tokens.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto callback_confirmed_it = tokens.find("oauth_callback_confirmed");
			if (callback_confirmed_it == tokens.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			if (callback_confirmed_it->second != "true")
				BOOST_THROW_EXCEPTION(oauth_exception());

			session_manager manager(database_, context, config_.session_manager);
			manager.begin_oauth(config_.site_name, token_it->second, token_secret_it->second);

			http_response& res = context.response();
			res.set_status_code(http_status_code::found);
			res.set_location(authorize_url_.get_raw() + "?oauth_token=" + encode_url(token_it->second));
		}

		virtual void on_get_callback(http_server_context& context) override
		{
			http_request& req = context.request();
			const auto& params = req.parameters();

			auto token_it = params.find("oauth_token");
			if (token_it == params.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto verifier_it = params.find("oauth_verifier");
			if (verifier_it == params.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto denied_it = params.find("denied");
			if (denied_it != params.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			session_manager manager(database_, context, config_.session_manager);
			std::string token_secret = manager.get().oauth_token_secret;
			if (!manager.verify_oauth_token(config_.site_name, token_it->second))
				BOOST_THROW_EXCEPTION(oauth_exception());

			parameters_type parameters;
			parameters["oauth_verifier"] = encode_url(verifier_it->second);

			tokens_type tokens = parse_tokens(signed_request(
				http_method::post, access_token_url_, parameters,
				token_it->second, token_secret));

			auto access_token_it = tokens.find("oauth_token");
			if (access_token_it == tokens.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto access_token_secret_it = tokens.find("oauth_token_secret");
			if (access_token_secret_it == tokens.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto user_id_it = tokens.find("user_id");
			if (user_id_it == tokens.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto screen_name_it = tokens.find("screen_name");
			if (screen_name_it == tokens.end())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto user_id_info = get_user_id_info(access_token_it->second, access_token_secret_it->second);

			manager.sign_in(config_.site_name, user_id_info.first, user_id_info.second);
		}

	protected:

		typedef std::map<std::string, std::string> parameters_type;
		typedef std::unordered_map<std::string, std::string> tokens_type;

		virtual std::pair<std::string, user_info> get_user_id_info(
			const std::string& access_token,
			const std::string& access_token_secret) = 0;

		std::string signed_request(
			http_method method,
			const url& url,
			const parameters_type& parameters,
			const std::string& token = std::string(),
			const std::string& token_secret = std::string()) const
		{
			BOOST_ASSERT(method == http_method::get || method == http_method::post);

			std::string nonce = boost::lexical_cast<std::string>(secure_random::get().uuid());
			std::string timestamp = boost::lexical_cast<std::string>((boost::posix_time::second_clock::universal_time() - epoch()).total_seconds());
			std::string encoded_token = encode_url(token);
			std::string encoded_token_secret = encode_url(token_secret);

			parameters_type params = parameters;

			params["oauth_consumer_key"] = encoded_consumer_key_;
			params["oauth_nonce"] = nonce;
			params["oauth_signature_method"] = "HMAC-SHA1";
			params["oauth_timestamp"] = timestamp;
			if (!encoded_token.empty())
				params["oauth_token"] = encoded_token;
			params["oauth_version"] = "1.0";

			std::string result;

			http_client_config config;
			config.ssl_context = ssl_context_.get();

			http_client client(service_, url, config);

			http_request& req = client.request();
			req.set_method(method);

			std::stringstream parameters_str;
			for (auto it = parameters.begin(); it != parameters.end(); ++it)
			{
				if (it != parameters.begin())
					parameters_str << "&";
				parameters_str << it->first;
				parameters_str << "=";
				parameters_str << it->second;
			}

			if (method == http_method::get)
				req.set_path(url.get_path() + '?' + parameters_str.str());

			std::string signature = get_signature(method, encode_url(url.get_raw()), params, encoded_token_secret);
			params["oauth_signature"] = encode_url(signature);

			std::ostringstream authorization;
			authorization << "OAuth ";

			for (auto it = params.begin(); it != params.end(); ++it)
			{
				if (it != params.begin())
					authorization << ",";
				authorization << it->first << "=\"" << it->second << "\"";
			}

			req.set_authorization(authorization.str());

			if (method == http_method::post)
				req.set_content_length(get_size(parameters_str));

			client.write_headers();

			if (method == http_method::post)
				forward(parameters_str, client.request_stream());

			client.finish_write();
			client.read_headers();

			result = read_until_end(client.response_stream());
			client.finish_read();

			http_status_code status_code = client.response().get_status_code();
			if (status_code != http_status_code::ok)
				BOOST_THROW_EXCEPTION(oauth_exception());

			return result;
		}

	private:

		std::string get_signature(
			http_method method,
			const std::string& encoded_url,
			const parameters_type& parameters,
			const std::string& encoded_token_secret = std::string()) const
		{
			std::ostringstream parameters_str;

			for (auto it = parameters.begin(); it != parameters.end(); ++it)
			{
				if (it != parameters.begin())
					parameters_str << "&";
				parameters_str << it->first;
				parameters_str << "=";
				parameters_str << it->second;
			}

			std::ostringstream base_str;
			base_str << method;
			base_str << "&" << encoded_url;
			base_str << "&" << encode_url(parameters_str.str());

			std::string key = encoded_consumer_secret_ + '&' + encoded_token_secret;

			return encode_base64(hash_sha1hmac(base_str.str(), key));
		}

		tokens_type parse_tokens(const std::string& str)
		{
			tokens_type result;

			typedef boost::char_separator<char> separator_type;
			typedef boost::tokenizer<separator_type> tokenizer_type;

			separator_type separator("&");
			tokenizer_type tokenizer(str, separator);

			for (const std::string& key_value : tokenizer)
			{
				std::size_t pos_equal = key_value.find('=');
				if (pos_equal == std::string::npos)
					BOOST_THROW_EXCEPTION(oauth_exception());

				std::string key(key_value.begin(), key_value.begin() + pos_equal);
				std::string value(key_value.begin() + pos_equal + 1, key_value.end());

				result.insert(std::make_pair(std::move(key), std::move(value)));
			}

			return result;
		}

		boost::asio::io_service& service_;
		database& database_;
		oauth_10_config config_;

		std::string encoded_callback_url_;
		std::string encoded_consumer_key_;
		std::string encoded_consumer_secret_;

		url request_token_url_;
		url authorize_url_;
		url access_token_url_;

		std::unique_ptr<boost::asio::ssl::context> ssl_context_;

	};

}
