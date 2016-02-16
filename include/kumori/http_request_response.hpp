#pragma once
#include "http_cookie.hpp"

namespace kumori
{

	class http_request_response
	{
	public:

		bool is_connection_keep_alive()
		{
			return connection_keep_alive_;
		}

		void set_connection_keep_alive(bool value)
		{
			connection_keep_alive_ = value;
		}

		const std::string& get_content_type() const
		{
			return content_type_;
		}

		void set_content_type(std::string value)
		{
			content_type_ = std::move(value);
		}

		const boost::optional<std::streamsize>& get_content_length() const
		{
			return content_length_;
		}

		void set_content_length(const boost::optional<std::streamsize>& value)
		{
			content_length_ = value;
		}

		bool is_chunked() const
		{
			return chunked_;
		}

		void set_chunked(bool value)
		{
			chunked_ = value;
		}

		const http_cookie& cookie() const
		{
			return cookie_;
		}

		http_cookie& cookie()
		{
			return cookie_;
		}

	protected:

		http_request_response()
		{
		}

		~http_request_response()
		{
		}

	private:

		bool connection_keep_alive_ = true;
		std::string content_type_;
		boost::optional<std::streamsize> content_length_;
		bool chunked_ = false;
		http_cookie cookie_;

	};

}
