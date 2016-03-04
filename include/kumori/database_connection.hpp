#pragma once
#include "serialization.hpp"

namespace kumori
{

	class database_connection : boost::noncopyable
	{
	public:

		virtual ~database_connection()
		{
		}

		virtual void begin() = 0;
		virtual void commit() = 0;
		virtual void rollback() = 0;

		virtual boost::optional<std::string> get(const std::string& key) = 0;
		virtual void set(const std::string& key, const std::string& value) = 0;

		template <class Value, class Key, class... Keys>
		boost::optional<Value> get(const Key& key, const Keys&... keys)
		{
			output_key_archive oa;
			oa(key, keys...);

			auto str = get(oa.str());
			if (!str)
				return boost::none;

			Value value;

			input_value_archive ia(std::move(*str));
			ia(value);

			return value;
		}

		template <class Arg1, class Arg2, class... Args>
		void set(const Arg1& arg1, const Arg2& arg2, const Args&... args)
		{
			output_key_archive oka;
			output_value_archive ova;
			set_impl(oka, ova, arg1, arg2, args...);

			set(oka.str(), ova.str());
		}

	private:

		template <class Key, class... Args>
		void set_impl(output_key_archive& oka, output_value_archive& ova, const Key& key, const Args&... args)
		{
			oka(key);
			set_impl(oka, ova, args...);
		}

		template <class Value>
		void set_impl(output_key_archive& /*oka*/, output_value_archive& ova, const Value& value)
		{
			ova(value);
		}

	};

	template <class Value, class Key, class... Keys>
	boost::optional<Value> get(database_connection& connection, const Key& key, const Keys&... keys)
	{
		return connection.template get<Value>(key, keys...);
	}

}
