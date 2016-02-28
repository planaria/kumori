#pragma once
#include "enum_hash.hpp"

namespace kumori
{

	enum class http_status_code : std::uint32_t
	{
		continue_ = 100,

		ok = 200,

		moved_permanently = 301,
		found = 302,
		not_modified = 304,

		bad_request = 400,
		unauthorized = 401,
		forbidden = 403,
		not_found = 404,
		method_not_allowed = 405,

		request_entity_too_large = 413,
		request_uri_too_long = 414,
		expectation_failed = 417,

		internal_service_error = 500,
		not_implemented = 501,
		http_version_not_supported = 505,

		end,
	};

	inline http_status_code to_http_status_code(const std::string& str)
	{
		if (str.size() == 3)
		{
			switch (str[0])
			{
			case '1':
				switch (str[1])
				{
				case '0':
					switch (str[2])
					{
					case '0':
						return http_status_code::continue_;
					}
					break;
				}
				break;
			case '2':
				switch (str[1])
				{
				case '0':
					switch (str[2])
					{
					case '0':
						return http_status_code::ok;
					}
					break;
				}
				break;
			case '3':
				if (str[1] == '0')
				{
					switch (str[2])
					{
					case '1':
						return http_status_code::moved_permanently;
					case '2':
						return http_status_code::found;
					case '4':
						return http_status_code::not_modified;
					}
				}
				break;
			case '4':
				switch (str[1])
				{
				case '0':
					switch (str[2])
					{
					case '0':
						return http_status_code::bad_request;
					case '1':
						return http_status_code::unauthorized;
					case '3':
						return http_status_code::forbidden;
					case '4':
						return http_status_code::not_found;
					case '5':
						return http_status_code::method_not_allowed;
					}
					break;
				case '1':
					switch (str[2])
					{
					case '3':
						return http_status_code::request_entity_too_large;
					case '4':
						return http_status_code::request_uri_too_long;
					case '7':
						return http_status_code::expectation_failed;
					}
					break;
				}
				break;
			case '5':
				switch (str[1])
				{
				case '0':
					switch (str[2])
					{
					case '0':
						return http_status_code::internal_service_error;
					case '1':
						return http_status_code::not_implemented;
					case '5':
						return http_status_code::http_version_not_supported;
					}
					break;
				}
				break;
			}
		}

		return http_status_code::end;
	}

	inline std::ostream& operator <<(std::ostream& stream, http_status_code code)
	{
		switch (code)
		{
		case http_status_code::continue_:
			stream << "100 Continue";
			break;
		case http_status_code::ok:
			stream << "200 OK";
			break;
		case http_status_code::moved_permanently:
			stream << "301 Moved Permanently";
			break;
		case http_status_code::found:
			stream << "302 Found";
			break;
		case http_status_code::not_modified:
			stream << "304 Not Modified";
			break;
		case http_status_code::bad_request:
			stream << "400 Bad Request";
			break;
		case http_status_code::unauthorized:
			stream << "401 Unauthorized";
			break;
		case http_status_code::forbidden:
			stream << "403 Forbidden";
			break;
		case http_status_code::not_found:
			stream << "404 Not Found";
			break;
		case http_status_code::method_not_allowed:
			stream << "405 Method Not Allowed";
			break;
		case http_status_code::request_entity_too_large:
			stream << "413 Request Entity Too Large";
			break;
		case http_status_code::request_uri_too_long:
			stream << "414 Request-URI Too Large";
			break;
		case http_status_code::expectation_failed:
			stream << "417 Expectation Failed";
			break;
		case http_status_code::internal_service_error:
			stream << "500 Internal Server Error";
			break;
		case http_status_code::not_implemented:
			stream << "501 Not Implemented";
			break;
		case http_status_code::http_version_not_supported:
			stream << "505 HTTP Version Not Supported";
			break;
		default:
			BOOST_ASSERT(false);
		}

		return stream;
	}

}

namespace std
{

	template <>
	struct hash<kumori::http_status_code>
		: kumori::enum_hash
	{
	};

}
