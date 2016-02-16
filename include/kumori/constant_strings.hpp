#pragma once

namespace kumori
{

	class constant_strings
	{
	public:

		static const std::string& crlf()
		{
			static std::string result("\r\n");
			return result;
		}

		static const std::string& http()
		{
			static std::string result("HTTP");
			return result;
		}

		static const std::string& http_version()
		{
			static std::string result("1.1");
			return result;
		}

		static const std::string& protocol_end()
		{
			static std::string result("://");
			return result;
		}

	private:

		constant_strings()
		{
		}

	};

}
