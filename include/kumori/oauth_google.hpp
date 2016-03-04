#pragma once
#include "oauth_20.hpp"
#include "oauth_google_config.hpp"

namespace kumori
{

	class oauth_google : public oauth_20
	{
	public:

		oauth_google(boost::asio::io_service& service, database& database, const oauth_google_config& config)
			: oauth_20(service, database, config)
			, service_(service)
			, me_url_(config.me_url)
		{
			if (!config.api_certificate.empty())
			{
				ssl_context_api_ = std::make_unique<boost::asio::ssl::context>(service, boost::asio::ssl::context::sslv23);
				ssl_context_api_->add_certificate_authority(boost::asio::const_buffer(config.api_certificate.data(), config.api_certificate.size()));
			}
		}

	protected:

		virtual std::pair<std::string, user_info> get_user_id_info(
			const std::string& access_token)
		{
			std::string response_str;

			{
				http_client_config config;
				config.ssl_context = ssl_context_api_.get();

				http_client client(service_, me_url_, config);

				http_request& req = client.request();
				req.set_method(http_method::get);
				req.set_path(me_url_.get_path() + "?access_token=" + encode_url(access_token));

				client.write_headers();
				client.finish_write();
				client.read_headers();

				response_str = read_until_end(client.response_stream());

				client.finish_read();

				http_status_code status_code = client.response().get_status_code();
				if (status_code != http_status_code::ok)
					BOOST_THROW_EXCEPTION(oauth_exception());
			}

			picojson::value value;
			CHECK_PICOJSON(parse(value, response_str));
			if (!value.is<picojson::object>())
				BOOST_THROW_EXCEPTION(oauth_exception());
			const picojson::object& tree = value.get<picojson::object>();

			std::string id = get_or_empty(tree, "id");
			if(id.empty())
				BOOST_THROW_EXCEPTION(oauth_exception());

			user_info info;
			info.name = get_or_empty(tree, "displayName");
			if(info.name.empty())
				BOOST_THROW_EXCEPTION(oauth_exception());

			auto image_it = tree.find("image");
			if (image_it != tree.end())
			{
				const picojson::object& image_tree = image_it->second.get<picojson::object>();
				info.icon = get_or_empty(image_tree, "url");
			}

			return std::make_pair(std::move(id), std::move(info));
		}

	private:

		boost::asio::io_service& service_;

		url me_url_;
		std::unique_ptr<boost::asio::ssl::context> ssl_context_api_;

	};

}
