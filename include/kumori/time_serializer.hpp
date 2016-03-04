#pragma once
#include "serialization_traits.hpp"
#include "serialize_split.hpp"
#include "date_time_util.hpp"

namespace kumori
{

	template <>
	struct serialization_traits<boost::posix_time::time_duration>
	{
		static const bool primitive = false;
		static const std::uint32_t version = 0;
	};

	template <class Archive>
	void serialize(Archive& archive, boost::posix_time::time_duration& value, std::uint32_t version)
	{
		serialize_split(archive, value, version);
	}

	template <class Archive>
	void save(Archive& archive, const boost::posix_time::time_duration& value, std::uint32_t version)
	{
		archive(value.total_milliseconds());
	}

	template <class Archive>
	void load(Archive& archive, boost::posix_time::time_duration& value, std::uint32_t version)
	{
		boost::posix_time::time_duration::tick_type ms;
		archive(ms);
		value = boost::posix_time::milliseconds(ms);
	}

	template <>
	struct serialization_traits<boost::posix_time::ptime>
	{
		static const bool primitive = false;
		static const std::uint32_t version = 0;
	};

	template <class Archive>
	void serialize(Archive& archive, boost::posix_time::ptime& value, std::uint32_t version)
	{
		serialize_split(archive, value, version);
	}

	template <class Archive>
	void save(Archive& archive, const boost::posix_time::ptime& value, std::uint32_t version)
	{
		archive(value - epoch());
	}

	template <class Archive>
	void load(Archive& archive, boost::posix_time::ptime& value, std::uint32_t version)
	{
		boost::posix_time::time_duration d;
		archive(d);
		value = epoch() + d;
	}

}
