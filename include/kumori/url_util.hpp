#pragma once
#include "string_util.hpp"
#include "char_table.hpp"

namespace kumori
{

	template <class Range>
	std::string encode_url(const Range& range)
	{
		static char_table is_valid([](char c)
		{
			if ('a' <= c && c <= 'z')
				return true;
			if ('A' <= c && c <= 'Z')
				return true;
			if ('0' <= c && c <= '9')
				return true;

			return c == '-' || c == '_' || c == '.' || c == '~';
		});

		std::string result;
		result.reserve(std::size(range));

		for (char c : range)
		{
			if (is_valid(c))
			{
				result.push_back(c);
			}
			else
			{
				result.push_back('%');
				result.push_back(to_hex_character((c & 0xf0) >> 4));
				result.push_back(to_hex_character(c & 0x0f));
			}
		}

		return result;
	}

	template <class Iterator>
	std::string encode_url(Iterator begin, Iterator end)
	{
		return encode_url(boost::make_iterator_range(begin, end));
	}

	template <class Range>
	bool decode_url(const Range& range, std::string& result)
	{
		struct hex2code
		{
			bool operator ()(char& c) const
			{
				if ('0' <= c && c <= '9')
					c -= '0';
				else if ('a' <= c && c <= 'f')
					c = c - 'a' + 10;
				else if ('A' <= c && c <= 'F')
					c = c - 'A' + 10;
				else
					return false;

				return true;
			}
		};

		result.clear();
		result.reserve(boost::size(range));

		auto end = boost::end(range);
		for (auto it = boost::begin(range); it != end; ++it)
		{
			char c = *it;

			if (c == '%')
			{
				char hi = *++it;
				char lo = *++it;
				if (!hex2code()(hi))
					return false;
				if (!hex2code()(lo))
					return false;
				result.push_back((hi << 4) + lo);
			}
			else
			{
				result.push_back(c);
			}
		}

		return true;
	}

	template <class Iterator>
	bool decode_url(Iterator begin, Iterator end, std::string& result)
	{
		return decode_url(boost::make_iterator_range(begin, end), result);
	}

	inline bool is_valid_path(const std::string& path)
	{
		if (path.empty())
			return false;
		if (path.front() != '/')
			return false;

		static char_table is_valid([](char c)
		{
			if ('a' <= c && c <= 'z')
				return true;
			if ('A' <= c && c <= 'Z')
				return true;
			if ('0' <= c && c <= '9')
				return true;

			switch (c)
			{
			case '-':
			case '_':
			case '.':
			case '!':
			case '~':
			case '*':
			case '\'':
			case '(':
			case ')':
			case ';':
			case '/':
			case '?':
			case ':':
			case '@':
			case '&':
			case '=':
			case '+':
			case '$':
			case ',':
			case '%':
			case '#':
				return true;
			}

			return false;
		});

		if (std::find_if(path.begin(), path.end(), std::not1(is_valid)) != path.end())
			return false;

		return true;
	}

}
