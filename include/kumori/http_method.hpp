#pragma once

namespace kumori
{

	enum class http_method
	{
		options,
		get,
		head,
		post,
		put,
		delete_,
		trace,
		connect,
		end
	};

	static const std::size_t maximum_method_length = 7;

	inline http_method to_http_method(const std::string& str)
	{
		switch (str.size())
		{
		case 3:
			switch (str[0])
			{
			case 'G':
				if (str != "GET")
					return http_method::end;
				return http_method::get;
			case 'P':
				if (str != "PUT")
					return http_method::end;
				return http_method::put;
			}
			break;
		case 4:
			switch (str[0])
			{
			case 'H':
				if (str != "HEAD")
					return http_method::end;
				return http_method::head;
			case 'P':
				if (str != "POST")
					return http_method::end;
				return http_method::post;
			}
			break;
		case 5:
			if (str != "TRACE")
				return http_method::end;
			return http_method::trace;
		case 6:
			if (str != "DELETE")
				return http_method::end;
			return http_method::delete_;
		case 7:
			switch (str[0])
			{
			case 'O':
				if (str != "OPTIONS")
					return http_method::end;
				return http_method::options;
			case 'C':
				if (str != "CONNECT")
					return http_method::end;
				return http_method::connect;
			}
			break;
		}

		return http_method::end;
	}

	inline std::ostream& operator <<(std::ostream& stream, http_method method)
	{
		switch (method)
		{
		case http_method::options:
			stream << "OPTIONS";
			break;
		case http_method::get:
			stream << "GET";
			break;
		case http_method::head:
			stream << "HEAD";
			break;
		case http_method::post:
			stream << "POST";
			break;
		case http_method::put:
			stream << "PUT";
			break;
		case http_method::delete_:
			stream << "DELETE";
			break;
		case http_method::trace:
			stream << "TRACE";
			break;
		case http_method::connect:
			stream << "CONNECT";
			break;
		default:
			BOOST_ASSERT(false);
		}

		return stream;
	}

}
