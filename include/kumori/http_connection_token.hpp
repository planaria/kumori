#pragma once

namespace kumori
{

	enum class http_connection_token
	{
		close,
		keep_alive,
		end,
	};

	inline http_connection_token to_http_connection_token(const std::string& str)
	{
		switch (str.size())
		{
		case 5:
			if (boost::algorithm::iequals(str, "Close"))
				return http_connection_token::close;
			break;
		case 10:
			if (boost::algorithm::iequals(str, "Keep-Alive"))
				return http_connection_token::keep_alive;
			break;
		}

		return http_connection_token::end;
	}

}
