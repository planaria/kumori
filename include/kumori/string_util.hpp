#pragma once

namespace kumori
{

	inline bool to_integer(const std::string& str, int& result, int base = 10)
	{
		std::size_t pos;
		int result_ = std::stoi(str, &pos, base);
		if (pos != str.size())
			return false;
		result = result_;
		return true;
	}

	inline bool to_integer(const std::string& str, long& result, int base = 10)
	{
		std::size_t pos;
		long result_ = std::stol(str, &pos, base);
		if (pos != str.size())
			return false;
		result = result_;
		return true;
	}

	inline bool to_integer(const std::string& str, long long& result, int base = 10)
	{
		std::size_t pos;
		long long result_ = std::stoll(str, &pos, base);
		if (pos != str.size())
			return false;
		result = result_;
		return true;
	}

	inline bool to_integer(const std::string& str, unsigned int& result, int base = 10)
	{
		std::size_t pos;
		unsigned long result_ = std::stoul(str, &pos, base);
		if (pos != str.size())
			return false;
		if (result_ > std::numeric_limits<std::size_t>::max())
			return false;
		result = result_;
		return true;
	}

	inline bool to_integer(const std::string& str, unsigned long& result, int base = 10)
	{
		std::size_t pos;
		unsigned long result_ = std::stoul(str, &pos, base);
		if (pos != str.size())
			return false;
		result = result_;
		return true;
	}

	inline bool to_integer(const std::string& str, unsigned long long& result, int base = 10)
	{
		std::size_t pos;
		unsigned long long result_ = std::stoull(str, &pos, base);
		if (pos != str.size())
			return false;
		result = result_;
		return true;
	}

	inline char to_hex_character(char n)
	{
		BOOST_ASSERT(n < 16);
		static const char* table = "0123456789ABCDEF";
		return table[static_cast<int>(n)];
	}

	template <class T>
	std::string to_hex(T n, typename std::enable_if<std::is_unsigned<T>::value>::type* = 0)
	{
		std::string result;

		bool output = false;
		for (int i = std::numeric_limits<T>::digits - 4; i >= 0; i -= 4)
		{
			int nibble = (n >> i) & 0xf;
			output |= !!nibble;
			if (output)
				result.push_back(to_hex_character(nibble));
		}

		return result;
	}

}
