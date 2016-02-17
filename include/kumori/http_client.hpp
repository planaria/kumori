#pragma once
#include "client.hpp"
#include "http_context.hpp"
#include "http_client_config.hpp"
#include "chunked_decoder.hpp"
#include "limit_length_filter.hpp"
#include "string_util.hpp"
#include "stream_util.hpp"
#include "constant_strings.hpp"
#include "http_parse_exception.hpp"

namespace kumori
{

	class http_client : public http_context<http_client>
	{
	public:

		http_client(boost::asio::io_service& service, const std::string& host, const std::string& port, const std::string& path, const http_client_config& config = http_client_config())
			: http_context(client_.stream(), config)
			, client_(service, host, port, config)
			, config_(config)
		{
			http_request& req = request();
			req.set_host(host);
			req.set_path(path);
		}

		std::ostream& request_stream()
		{
			return request_stream_;
		}

		std::istream& response_stream()
		{
			return response_stream_;
		}

		void write_headers()
		{
			http_request& req = request();
			std::iostream& stream = get_stream();

			BOOST_ASSERT(req.get_method() != http_method::end);
			BOOST_ASSERT(!req.get_path().empty());

			stream << req.get_method() << " " << req.get_path() << " HTTP/1.1\r\n";
			
			BOOST_ASSERT(!req.get_host().empty());

			stream << "Host: " << req.get_host() << "\r\n";

			if (!req.get_authorization().empty())
				stream << "Authorization: " << req.get_authorization() << "\r\n";

			if (!req.get_content_type().empty())
				stream << "Content-Type: " << req.get_content_type() << "\r\n";

			if (req.get_method() == http_method::post)
			{
				if (req.get_content_length())
				{
					BOOST_ASSERT(!req.is_chunked());
					stream << "Content-Length: " << *req.get_content_length() << "\r\n";
				}
				else if (req.is_chunked())
				{
					stream << "Transfer-Encoding: chunked\r\n";
				}
			}
			else
			{
				BOOST_ASSERT(!req.get_content_length() && !req.is_chunked());
			}

			req.cookie().write_request(stream);

			stream << "\r\n";
		}

		void finish_write()
		{
			http_request& req = request();
			std::iostream& stream = get_stream();

			std::size_t content_size = get_size(request_stream_);

			if (req.get_method() == http_method::post)
			{
				if (req.get_content_length())
				{
					BOOST_ASSERT(!req.is_chunked());
					BOOST_ASSERT(content_size == *req.get_content_length());
					forward(request_stream_, stream, *req.get_content_length());
				}
				else if (req.is_chunked())
				{
					if (content_size != 0)
					{
						stream << to_hex(content_size) << "\r\n";
						forward(request_stream_, stream, content_size);
						stream << "\r\n0\r\n\r\n";
					}
					else
					{
						stream << "0\r\n\r\n";
					}
				}
				else
				{
					BOOST_ASSERT(content_size == 0);
				}
			}
			else
			{
				BOOST_ASSERT(!req.get_content_length() && !req.is_chunked());
				BOOST_ASSERT(content_size == 0);
			}

			stream << std::flush;
		}

		void read_headers()
		{
			http_response& res = response();
			std::iostream& stream = get_stream();

			std::string token;

			if (!read_token(token, '/', 4))
				BOOST_THROW_EXCEPTION(http_parse_exception());
			if (token != constant_strings::http())
				BOOST_THROW_EXCEPTION(http_parse_exception());

			if (!read_token(token, ' ', 3))
				BOOST_THROW_EXCEPTION(http_parse_exception());
			if (token != "1.0" && token != "1.1")
				BOOST_THROW_EXCEPTION(http_parse_exception());

			if (!read_token(token, ' ', 3))
				BOOST_THROW_EXCEPTION(http_parse_exception());
			res.set_status_code(to_http_status_code(token));

			ignore_until_crlf();

			read_entity_headers();

			if (!!res.get_content_length() == !!res.is_chunked())
				BOOST_THROW_EXCEPTION(http_parse_exception());

			if (res.get_content_length())
				response_stream_.push(limit_length_filter(*res.get_content_length()));
			else if (res.is_chunked())
				response_stream_.push(chunked_decoder());

			response_stream_.push(stream, 0, 0);
		}

		void finish_read()
		{
			ignore_until_end(response_stream_);
		}

	private:

		friend class http_context<http_client>;

		void set_header(http_entity_header header, const std::string& value)
		{
			http_response& res = response();

			typedef boost::char_separator<char> SeparatorType;
			typedef boost::tokenizer<SeparatorType> TokenizerType;

			static SeparatorType separator(" ,");

			switch (header)
			{
			case http_entity_header::transfer_encoding:
				for (const std::string& str : TokenizerType(value, separator))
				{
					if (boost::algorithm::iequals(str, "chunked"))
						res.set_chunked(true);
				}
				break;
			case http_entity_header::connection:
				for (const std::string& str : TokenizerType(value, separator))
				{
					switch (to_http_connection_token(str))
					{
					case http_connection_token::close:
						res.set_connection_keep_alive(false);
						break;
					case http_connection_token::keep_alive:
						res.set_connection_keep_alive(true);
						break;
					default:
						break;
					}
				}
				break;
			case http_entity_header::content_length:
			{
				std::streamsize contentLength;
				if (!to_integer(value, contentLength))
					BOOST_THROW_EXCEPTION(http_parse_exception());
				res.set_content_length(contentLength);
			}
			break;
			case http_entity_header::set_cookie:
				if (!res.cookie().parse(value))
					BOOST_THROW_EXCEPTION(http_parse_exception());
				break;
			default:
				break;
			}
		}

		void on_error()
		{
			BOOST_THROW_EXCEPTION(http_parse_exception());
		}

		client client_;
		http_client_config config_;

		std::stringstream request_stream_;
		boost::iostreams::filtering_istream response_stream_;

	};

}
