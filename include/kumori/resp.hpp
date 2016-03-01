#pragma once
#include "redis_exception.hpp"

namespace kumori
{

	class resp_nil
	{
	};

	class resp_string
	{
	public:

		explicit resp_string(std::string value_)
			: value(std::move(value_))
		{
		}

		std::string value;

	};

	class resp_error
	{
	public:

		explicit resp_error(std::string message_)
			: message(std::move(message_))
		{
		}

		std::string message;

	};

	class resp_integer
	{
	public:

		explicit resp_integer(std::int64_t value_)
			: value(value_)
		{
		}

		std::int64_t value;

	};

	template <class Resp>
	class resp_array_
	{
	public:

		explicit resp_array_(std::vector<Resp> values_)
			: values(std::move(values_))
		{
		}

		std::vector<Resp> values;

	};

	typedef boost::make_recursive_variant<
		resp_nil,
		resp_string,
		resp_error,
		resp_integer,
		resp_array_<boost::recursive_variant_>
	>::type resp;

	typedef resp_array_<resp> resp_array;

	inline void check_error(const resp& r)
	{
		if (r.type() == typeid(resp_error))
			BOOST_THROW_EXCEPTION(redis_exception(boost::get<const resp_error&>(r).message));
	}

	inline std::string as_string(resp r)
	{
		check_error(r);
		if (r.type() == typeid(resp_string))
			return std::move(boost::get<resp_string&>(r).value);

		BOOST_THROW_EXCEPTION(redis_exception("invalid result type"));
	}

	inline boost::optional<std::string> as_string_optional(resp r)
	{
		check_error(r);
		if (r.type() == typeid(resp_string))
			return std::move(boost::get<resp_string&>(r).value);
		if (r.type() == typeid(resp_nil))
			return boost::none;

		BOOST_THROW_EXCEPTION(redis_exception("invalid result type"));
	}

	inline std::int64_t as_integer(const resp& r)
	{
		check_error(r);
		if (r.type() == typeid(resp_integer))
			return boost::get<const resp_integer&>(r).value;

		BOOST_THROW_EXCEPTION(redis_exception("invalid result type"));
	}

	inline boost::optional<std::int64_t> as_integer_optional(const resp& r)
	{
		check_error(r);
		if (r.type() == typeid(resp_integer))
			return boost::get<const resp_integer&>(r).value;
		if (r.type() == typeid(resp_nil))
			return boost::none;

		BOOST_THROW_EXCEPTION(redis_exception("invalid result type"));
	}

	inline std::vector<resp> as_array(resp r)
	{
		check_error(r);
		if (r.type() == typeid(resp_array))
			return std::move(boost::get<resp_array&>(r).values);

		BOOST_THROW_EXCEPTION(redis_exception("invalid result type"));
	}

}
