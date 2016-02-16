#pragma once

namespace kumori
{

	enum class protocol
	{
		http,
		https,
		end
	};

	inline protocol to_protocol(const std::string& str)
	{
		switch (str.size())
		{
		case 4:
			if (str == "http")
				return protocol::http;
			break;
		case 5:
			if (str == "https")
				return protocol::https;
			break;
		}

		return protocol::end;
	}

	inline std::ostream& operator <<(std::ostream& stream, protocol p)
	{
		switch (protocol)
		{
		case protocol::http:
			stream << "http";
			break;
		case protocol::https:
			stream << "https";
			break;
		case protocol::end:
		default:
			stream << "unknown";
			break;
		}

		return stream;
	}

}
