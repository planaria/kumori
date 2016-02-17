#pragma once
#include "date_time_util.hpp"

namespace kumori
{

	class http_cookie
	{
	public:

		bool set(std::string name, std::string value)
		{
			return map_.insert(std::make_pair(std::move(name), std::move(value))).second;
		}

		template <class T>
		void set(std::string name, const T& value)
		{
			set(std::move(name), boost::lexical_cast<std::string>(value));
		}

		bool get(const std::string& name, std::string& value) const
		{
			auto it = map_.find(name);
			if (it == map_.end())
				return false;

			value = it->second;
			return true;
		}

		template <class T>
		bool get(const std::string& name, T& value) const
		{
			std::string str;
			if (!get(name, str))
				return false;

			value = boost::lexical_cast<T>(str);
			return true;
		}

		template <class T>
		T get_or_default(const std::string& name, const T& default_value) const
		{
			T result;
			if (!get(name, result))
				return default_value;
			return result;
		}

		void clear()
		{
			map_.clear();
		}

		bool is_empty() const
		{
			return map_.empty();
		}

		bool parse(const std::string& str)
		{
			clear();

			typedef boost::char_separator<char> separator_type;
			typedef boost::tokenizer<separator_type> tokenizer_type;

			separator_type separator(";");
			tokenizer_type tokenizer(str, separator);

			for (const std::string& keyValue : tokenizer)
			{
				std::size_t posEqual = keyValue.find('=');
				if (posEqual != std::string::npos)
				{
					std::string key(keyValue.begin(), keyValue.begin() + posEqual);
					std::string value(keyValue.begin() + posEqual + 1, keyValue.end());
					boost::trim(key);
					boost::trim(value);
					if (!set(std::move(key), std::move(value)))
						return false;
				}
				else
				{
					if (!set(boost::trim_copy(keyValue), std::string()))
						return false;
				}
			}

			return true;
		}

		void write_response(std::ostream& stream, const boost::posix_time::time_duration& life_time)
		{
			std::string expires;

			if (!map_.empty())
			{
				auto now = boost::posix_time::second_clock::universal_time();
				expires = format_rfc1123(now + life_time);
			}

			for (const auto& key_value : map_)
			{
				stream << "Set-Cookie: ";
				stream << key_value.first << "=" << key_value.second;
				stream << "; expires=" << expires;
				stream << "; path=/\r\n";
			}
		}

		void write_request(std::ostream& stream)
		{
			if (!map_.empty())
			{
				stream << "Cookie: ";

				for (auto it = map_.begin(); it != map_.end(); ++it)
				{
					if (it != map_.begin())
						stream << "; ";
					stream << it->first << "=" << it->second;
				}

				stream << "\r\n";
			}
		}

	private:

		std::unordered_map<std::string, std::string> map_;

	};

}
