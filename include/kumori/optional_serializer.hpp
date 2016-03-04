#pragma once
#include "serialization_traits.hpp"
#include "serialize_split.hpp"

namespace kumori
{

	template <class T>
	struct serialization_traits<boost::optional<T>>
	{
		static const bool primitive = false;
		static const std::uint32_t version = 0;
	};

	template <class Archive, class T>
	void serialize(Archive& archive, boost::optional<T>& value, std::uint32_t version)
	{
		serialize_split(archive, value, version);
	}

	template <class Archive, class T>
	void save(Archive& archive, const boost::optional<T>& value, std::uint32_t version)
	{
		bool has_value = !!value;
		archive(has_value);

		if (has_value)
			archive(*value);
	}

	template <class Archive, class T>
	void load(Archive& archive, boost::optional<T>& value, std::uint32_t version)
	{
		bool has_value;
		archive(has_value);

		if (has_value)
		{
			value = boost::in_place();
			archive(*value);
		}
		else
		{
			value = boost::none;
		}
	}

}
