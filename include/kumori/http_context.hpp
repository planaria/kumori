#pragma once
#include "http_request.hpp"
#include "http_response.hpp"
#include "http_config.hpp"

namespace kumori
{

	template <class Derived>
	class http_context : boost::noncopyable
	{
	public:

		const http_request& request() const
		{
			return request_;
		}

		http_request& request()
		{
			return request_;
		}

		const http_response& response() const
		{
			return response_;
		}

		http_response& response()
		{
			return response_;
		}

	protected:

		http_context(std::iostream& stream, const http_config& config)
			: stream_(stream)
			, config_(config)
		{
		}

		~http_context()
		{
		}

		std::iostream& get_stream()
		{
			return stream_;
		}

		void read_entity_headers()
		{
			std::string token;

			while (true)
			{
				if (stream_.peek() == '\r')
				{
					stream_.ignore();
					if (stream_.get() != '\n')
						derived().on_error();
					break;
				}

				http_entity_header header = http_entity_header::end;

				if (read_token(token, ':', maximum_request_header_length, true))
					header = to_http_entity_header(token);

				if (header != http_entity_header::end)
				{
					read_header_value(token, config_.maximum_header_length);
					derived().set_header(header, token);
				}
				else
				{
					ignore_header_value();
				}
			}
		}

		void ignore_header_value()
		{
			while (true)
			{
				if (stream_.get() == '\r')
				{
					if (stream_.get() != '\n')
						derived().on_error();
					switch (stream_.peek())
					{
					case ' ':
					case '\t':
						stream_.ignore();
						continue;
					}
					break;
				}
			}
		}

		void read_header_value(std::string& token, std::size_t limit)
		{
			token.clear();

			bool lws = false;

			while (true)
			{
				char c = stream_.get();

				switch (c)
				{
				case '\r':
					if (stream_.get() != '\n')
						derived().on_error();
					switch (stream_.peek())
					{
					case ' ':
					case '\t':
						stream_.ignore();
						lws = true;
						break;
					default:
						return;
					}
					break;
				case ' ':
				case '\t':
					lws = true;
					break;
				default:
					if (lws)
					{
						if (!token.empty())
						{
							if (token.size() == limit)
								derived().on_error();
							token.push_back(' ');
						}
						lws = false;
					}
					if (token.size() == limit)
						derived().on_error();
					token.push_back(c);
					break;
				}
			}
		}

		bool read_token(std::string& token, char delim, std::size_t limit, bool ignore_until = false)
		{
			token.clear();

			for (std::size_t i = 0; ; ++i)
			{
				char c = stream_.get();
				if (c == delim)
					return true;

				if (i == limit)
				{
					if (ignore_until)
						stream_.ignore(boost::integer_traits<std::streamsize>::const_max, delim);

					return false;
				}

				token.push_back(c);
			}
		}

		bool read_token(std::string& token, const std::string& delim, std::size_t limit)
		{
			BOOST_ASSERT(!delim.empty());
			limit += delim.size();

			token.clear();

			for (std::size_t i = 0; i < limit; ++i)
			{
				char c;
				stream_.get(c);
				token.push_back(c);

				if (boost::algorithm::ends_with(token, delim))
				{
					token.resize(token.size() - delim.size());
					return true;
				}
			}

			return false;
		}

		void ignore_until_crlf()
		{
			while (true)
			{
				if (stream_.get() == '\r')
				{
					if (stream_.get() != '\n')
						derived().on_error();
					return;
				}
			}
		}

	private:

		Derived& derived()
		{
			return static_cast<Derived&>(*this);
		}

		std::iostream& stream_;
		http_config config_;

		http_request request_;
		http_response response_;

	};

}
