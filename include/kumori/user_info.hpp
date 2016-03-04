#pragma once
#include "serialization.hpp"

namespace kumori
{

	struct user_info
	{
		static const std::uint32_t serialize_version = 0;

		std::string name;
		std::string icon;
	};

	template <class Archive>
	void serialize(Archive& archive, user_info& value, std::int32_t /*version*/)
	{
		archive(value.name);
		archive(value.icon);
	}

}
