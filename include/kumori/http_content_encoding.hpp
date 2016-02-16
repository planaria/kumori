#pragma once

namespace kumori
{

	enum class http_content_encoding
	{
		gzip,
		compress,
		deflate,
		identity,
		end
	};

	inline http_content_encoding to_http_content_encoding(const std::string& str)
	{
		switch (str.size())
		{
		case 4:
			if (boost::algorithm::iequals(str, "gzip"))
				return http_content_encoding::gzip;
			break;
		case 6:
			if (boost::algorithm::iequals(str, "x-gzip"))
				return http_content_encoding::gzip;
			break;
		case 7:
			if (boost::algorithm::iequals(str, "deflate"))
				return http_content_encoding::deflate;
			break;
		case 8:
			switch (std::toupper(str[0]))
			{
			case 'C':
				if (boost::algorithm::iequals(str, "compress"))
					return http_content_encoding::compress;
				break;
			case 'I':
				if (boost::algorithm::iequals(str, "identity"))
					return http_content_encoding::identity;
				break;
			}
			break;
		case 10:
			if (boost::algorithm::iequals(str, "x-compress"))
				return http_content_encoding::compress;
			break;
		}

		return http_content_encoding::end;
	}

	inline std::ostream& operator <<(std::ostream& stream, http_content_encoding encoding)
	{
		switch (encoding)
		{
		case http_content_encoding::gzip:
			stream << "gzip";
			break;
		case http_content_encoding::compress:
			stream << "compress";
			break;
		case http_content_encoding::deflate:
			stream << "deflate";
			break;
		case http_content_encoding::identity:
			stream << "identity";
			break;
		default:
			BOOST_ASSERT(false);
		}

		return stream;
	}

}
