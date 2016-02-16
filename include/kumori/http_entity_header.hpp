#pragma once

namespace kumori
{

	enum class http_entity_header
	{
		host,
		if_none_match,
		if_modified_since,
		accept,
		accept_encoding,
		expect,
		transfer_encoding,
		connection,
		content_length,
		cookie,
		set_cookie,
		x_csrf_token,
		end,
	};

	static const std::size_t maximum_request_header_length = 17;

	inline http_entity_header to_http_entity_header(const std::string& str)
	{
		switch (str.size())
		{
		case 4:
			if (boost::algorithm::iequals(str, "Host"))
				return http_entity_header::host;
			break;
		case 6:
			switch (std::toupper(str[0]))
			{
			case 'A':
				if (boost::algorithm::iequals(str, "Accept"))
					return http_entity_header::accept;
				break;
			case 'E':
				if (boost::algorithm::iequals(str, "Expect"))
					return http_entity_header::expect;
				break;
			case 'C':
				if (boost::algorithm::iequals(str, "Cookie"))
					return http_entity_header::cookie;
				break;
			}
			break;
		case 10:
			if (boost::algorithm::iequals(str, "Connection"))
				return http_entity_header::connection;
			if (boost::algorithm::iequals(str, "Set-Cookie"))
				return http_entity_header::set_cookie;
			break;
		case 12:
			if (boost::algorithm::iequals(str, "X-CSRF-Token"))
				return http_entity_header::x_csrf_token;
			break;
		case 13:
			if (boost::algorithm::iequals(str, "If-None-Match"))
				return http_entity_header::if_none_match;
			break;
		case 14:
			if (boost::algorithm::iequals(str, "Content-Length"))
				return http_entity_header::content_length;
			break;
		case 15:
			if (boost::algorithm::iequals(str, "Accept-Encoding"))
				return http_entity_header::accept_encoding;
			break;
		case 17:
			switch (std::toupper(str[0]))
			{
			case 'I':
				if (boost::algorithm::iequals(str, "If-Modified-Since"))
					return http_entity_header::if_modified_since;
				break;
			case 'T':
				if (boost::algorithm::iequals(str, "Transfer-Encoding"))
					return http_entity_header::transfer_encoding;
				break;
			}
			break;
		}

		return http_entity_header::end;
	}

	inline std::ostream& operator <<(std::ostream& stream, http_entity_header header)
	{
		switch (header)
		{
		case http_entity_header::host:
			stream << "Host";
			break;
		case http_entity_header::if_none_match:
			stream << "If-None-Match";
			break;
		case http_entity_header::if_modified_since:
			stream << "If-Modified-Since";
			break;
		case http_entity_header::accept:
			stream << "Accept";
			break;
		case http_entity_header::accept_encoding:
			stream << "Accept-Encoding";
			break;
		case http_entity_header::expect:
			stream << "Expect";
			break;
		case http_entity_header::transfer_encoding:
			stream << "Transfer-Encoding";
			break;
		case http_entity_header::connection:
			stream << "Connection";
			break;
		case http_entity_header::content_length:
			stream << "Content-Length";
			break;
		case http_entity_header::cookie:
			stream << "Cookie";
			break;
		case http_entity_header::set_cookie:
			stream << "Set-Cookie";
			break;
		default:
			BOOST_ASSERT(false);
		}

		return stream;
	}

}
