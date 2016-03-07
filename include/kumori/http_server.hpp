#pragma once
#include "server.hpp"
#include "http_server_config.hpp"
#include "http_server_context.hpp"
#include "null_content.hpp"

namespace kumori
{

	template <class Derived>
	class http_server : public server<http_server<Derived>>
	{
	private:

		typedef server<http_server<Derived>> base_type;

	public:

		explicit http_server(boost::asio::io_service& service, const http_server_config& config = http_server_config())
			: base_type(service, config)
			, config_(config)
		{
		}

		void on_connected(std::iostream& stream)
		{
			for (std::size_t keep_alive_count = config_.maximum_keep_alive_requests; keep_alive_count != 0; --keep_alive_count)
			{
				stream.exceptions(std::ios_base::badbit);

				if (stream.peek() == EOF)
				{
					stream.clear();
					break;
				}

				stream.exceptions(std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);

				http_server_context context(stream, config_);
				http_request& req = context.request();
				http_response& res = context.response();

				bool head = false;
				bool keep_alive = false;

				try
				{
					context.read_headers();

					switch (req.get_method())
					{
					case http_method::head:
						head = true;
						break;
					case http_method::get:
					case http_method::post:
						break;
					default:
						BOOST_THROW_EXCEPTION(http_exception(http_status_code::method_not_allowed));
					}

					if (req.is_connection_keep_alive())
					{
						res.set_keep_alive_requests(keep_alive_count);
						keep_alive = true;
					}

					context.validate_headers();

					try
					{
						switch (req.get_method())
						{
						case http_method::head:
						case http_method::get:
							derived().on_get(context, head);
							break;
						case http_method::post:
							derived().on_post(context);
							break;
						default:
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::method_not_allowed));
						}
					}
					catch(http_exception&)
					{
						throw;
					}
					catch (interrupted_exception&)
					{
						throw;
					}
					catch (std::exception&)
					{
						BOOST_THROW_EXCEPTION(http_exception(http_status_code::internal_service_error));
					}
				}
				catch (http_exception& e)
				{
					res.set_status_code(e.status_code());

					try
					{
						auto it = config_.error_document.find(e.status_code());
						if (it != config_.error_document.end())
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::not_found));

						req.set_path(it->second);

						derived().on_get(context, head);
					}
					catch (http_exception&)
					{
						null_content().on_get(context, head);
					}
				}

				context.finish_write();

				if (!keep_alive)
					break;
			}
		}

		void on_get(http_server_context& /*context*/, bool /*head*/)
		{
			BOOST_THROW_EXCEPTION(http_exception(http_status_code::method_not_allowed));
		}

		void on_post(http_server_context& /*context*/)
		{
			BOOST_THROW_EXCEPTION(http_exception(http_status_code::method_not_allowed));
		}

	private:

		Derived& derived()
		{
			return static_cast<Derived&>(*this);
		}

		http_server_config config_;

	};

}
