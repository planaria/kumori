#pragma once
#include "http_context.hpp"
#include "chunked_encoder.hpp"
#include "http_connection_token.hpp"
#include "stream_util.hpp"
#include "url_util.hpp"
#include "constant_strings.hpp"
#include "http_exception.hpp"

namespace kumori
{

	template <class Derived>
	class http_server;

	class http_server_context : public http_context<http_server_context>
	{
	public:

		http_server_context(std::iostream& stream, const http_server_config& config)
			: http_context(stream, config)
			, config_(config)
		{
		}

		void write_headers()
		{
			http_response& res = response();
			std::iostream& stream = get_stream();

			stream << "HTTP/1.1 " << res.get_status_code() << "\r\n";

			if (!res.get_location().empty())
				stream << "Location: " << res.get_location() << "\r\n";

			stream << "Date: " << format_rfc1123(boost::posix_time::second_clock::universal_time()) << "\r\n";

			if (res.has_vary())
			{
				stream << "Vary: ";

				bool found = false;

				for (std::size_t i = 0; i != static_cast<std::size_t>(http_entity_header::end); ++i)
				{
					if (!res.test_vary(static_cast<http_entity_header>(i)))
						continue;

					if (found)
						stream << ", ";
					else
						found = true;

					stream << static_cast<http_entity_header>(i);
				}

				stream << "\r\n";
			}

			if (res.get_keep_alive_requests() != 0)
			{
				int timeout = config_.keep_alive_timeout.total_seconds();
				stream << "Connection: Keep-Alive\r\n";
				stream << "Keep-Alive: timeout=" << timeout << ", max=" << res.get_keep_alive_requests() << "\r\n";

				if (!res.get_content_length())
				{
					stream << "Transfer-Encoding: chunked\r\n";
					response_stream_.push(chunked_encoder());
				}
			}
			else
			{
				stream << "Connection: close\r\n";
			}

			if (res.get_content_length())
				stream << "Content-Length: " << *res.get_content_length() << "\r\n";

			if (!res.get_mime_type().empty())
				stream << "Content-Type: " << res.get_mime_type() << "\r\n";

			if (res.get_content_encoding() != http_content_encoding::identity)
				stream << "Content-Encoding: " << res.get_content_encoding() << "\r\n";

			if (!res.get_e_tag().empty())
				stream << "ETag: " << res.get_e_tag() << "\r\n";

			if (!res.get_last_modified().empty())
				stream << "Last-Modified: " << res.get_last_modified() << "\r\n";

			res.cookie().write_response(stream, config_.session_life_time);

			stream << "\r\n";

			response_stream_.push(stream);
		}

		std::istream& request_stream()
		{
			return request_stream_;
		}

		std::ostream& response_stream()
		{
			return response_stream_;
		}

	private:

		template <class Derived>
		friend class http_server;

		friend class http_context<http_server_context>;

		void read_headers()
		{
			http_request& req = request();
			std::iostream& stream = get_stream();

			std::string token;

			if (!read_token(token, ' ', maximum_method_length))
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
			req.set_method(to_http_method(token));

			std::string original_path;

			if (!read_token(original_path, ' ', config_.maximum_path_length))
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
			if (!is_valid_path(original_path))
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));

			auto pathEnd = std::find(original_path.begin(), original_path.end(), '?');

			std::string path;
			if (!decode_url(original_path.begin(), pathEnd, path))
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
			req.set_path(std::move(path));

			if (pathEnd != original_path.end())
			{
				typedef boost::char_separator<char> SeparatorType;
				typedef boost::tokenizer<SeparatorType> TokenizerType;

				static SeparatorType separator("&");

				auto& parameters = req.parameters();

				parameters.clear();

				for (const std::string& param : TokenizerType(boost::next(pathEnd), original_path.end(), separator))
				{
					auto keyEnd = std::find(param.begin(), param.end(), '=');
					if (keyEnd != param.end())
					{
						std::string key;
						if (!decode_url(param.begin(), keyEnd, key))
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));

						std::string value;
						if (!decode_url(boost::next(keyEnd), param.end(), value))
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));

						parameters[key] = std::move(value);
					}
					else
					{
						parameters[param].clear();
					}
				}
			}

			if (!read_token(token, '/', 4))
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
			if (token != constant_strings::http())
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));

			if (!read_token(token, constant_strings::crlf(), 3))
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::http_version_not_supported));
			if (token != constant_strings::http_version())
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::http_version_not_supported));

			read_entity_headers();

			if (req.get_method() == http_method::post)
			{
				if (req.get_expect_continue())
					stream << "HTTP/1.1 " << http_status_code::continue_ << "\r\n\r\n" << std::flush;

				if (req.get_content_length() && req.is_chunked())
					BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));

				if (req.get_content_length())
				{
					if (*req.get_content_length() > config_.maximum_request_content_length)
						BOOST_THROW_EXCEPTION(http_exception(http_status_code::request_entity_too_large));
					forward(stream, request_stream_, *req.get_content_length());
				}
				else if (req.is_chunked())
				{
					std::streamsize contentLength = 0;

					while (true)
					{
						if (!read_token(token, constant_strings::crlf(), 8))
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));

						std::streamsize length;
						if (!to_integer(token, length, 16))
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));

						if (length == 0)
							break;

						contentLength += length;
						if (contentLength > config_.maximum_request_content_length)
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::request_entity_too_large));

						forward(stream, request_stream_, length);

						if (stream.get() != '\r')
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
						if (stream.get() != '\n')
							BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
					}

					read_entity_headers();
				}
			}
			else
			{
				if (req.get_content_length() || req.is_chunked())
					BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
			}
		}

		void validate_headers()
		{
			http_request& req = request();

			if (config_.strict_http && req.get_host().empty())
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
			if (req.get_unknown_expectation())
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::expectation_failed));
			if (!req.get_cached_date().empty() && to_posix_time(req.get_cached_date()) == boost::posix_time::not_a_date_time)
				BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
		}

		void finish_write()
		{
			response_stream_ << std::flush;
			boost::iostreams::close(response_stream_);
			get_stream() << std::flush;
		}

		void set_header(http_entity_header header, const std::string& value)
		{
			http_request& req = request();

			typedef boost::char_separator<char> SeparatorType;
			typedef boost::tokenizer<SeparatorType> TokenizerType;

			static SeparatorType separator(" ,");

			switch (header)
			{
			case http_entity_header::host:
				req.set_host(value);
				break;
			case http_entity_header::if_none_match:
				req.set_cached_e_tag(value);
				break;
			case http_entity_header::if_modified_since:
				req.set_cached_date(value);
				break;
			case http_entity_header::accept:
				break;
			case http_entity_header::accept_encoding:
				for (const std::string& str : TokenizerType(value, separator))
				{
					http_content_encoding encoding = to_http_content_encoding(str);
					if (encoding != http_content_encoding::end)
						req.set_accept_encoding(encoding);
				}
				break;
			case http_entity_header::expect:
				for (const std::string& str : TokenizerType(value, separator))
				{
					if (boost::algorithm::iequals(str, "100-continue"))
						req.set_expect_continue(true);
					else
						req.set_unknown_expectation(true);
				}
				break;
			case http_entity_header::transfer_encoding:
				for (const std::string& str : TokenizerType(value, separator))
				{
					if (boost::algorithm::iequals(str, "chunked"))
						req.set_chunked(true);
				}
				break;
			case http_entity_header::connection:
				for (const std::string& str : TokenizerType(value, separator))
				{
					switch (to_http_connection_token(str))
					{
					case http_connection_token::close:
						req.set_connection_keep_alive(false);
						break;
					case http_connection_token::keep_alive:
						req.set_connection_keep_alive(true);
						break;
					default:
						break;
					}
				}
				break;
			case http_entity_header::content_length:
			{
				std::streamsize content_length;
				if (!to_integer(value, content_length))
					BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
				req.set_content_length(content_length);
			}
			break;
			case http_entity_header::cookie:
				if (!req.cookie().parse(value))
					BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
				break;
			case http_entity_header::x_csrf_token:
				req.set_csrf_token(value);
				break;
			default:
				break;
			}
		}

		void on_error()
		{
			BOOST_THROW_EXCEPTION(http_exception(http_status_code::bad_request));
		}

		http_server_config config_;

		std::stringstream request_stream_;
		boost::iostreams::filtering_ostream response_stream_;

	};

}
