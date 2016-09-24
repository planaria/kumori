#pragma once

namespace kumori
{

	inline bool match_wildcard(const std::string& str, const std::string& pattern, bool case_sensitive)
	{
		std::string regex_pattern;
		regex_pattern.reserve(pattern.size() * 2);

		for (char c : pattern)
		{
			switch (c)
			{
			case '\\':
			case '^':
			case '.':
			case '$':
			case '|':
			case '(':
			case ')':
			case '[':
			case ']':
			case '+':
			case '/':
				regex_pattern.push_back('\\');
				regex_pattern.push_back(c);
				break;
			case '?':
				regex_pattern.push_back('.');
				break;
			case '*':
				regex_pattern.push_back('.');
				regex_pattern.push_back('*');
				break;
			default:
				regex_pattern.push_back(c);
				break;
			}
		}

		std::regex r(
			regex_pattern,
			case_sensitive
			? std::regex_constants::ECMAScript
			: std::regex_constants::ECMAScript | std::regex_constants::icase);

		return std::regex_match(str, r);
	}

	template <class InputIterator>
	inline bool match_wildcard(const std::string& str, InputIterator patterns_begin, InputIterator patterns_end, bool case_sensitive)
	{
		return std::find_if(patterns_begin, patterns_end,
			[&](const std::string& pattern)
		{
			return match_wildcard(str, pattern, case_sensitive);
		}) != patterns_end;
	}

}
