#pragma once
#include "serialization_traits.hpp"

namespace kumori
{

	template <>
	struct serialization_traits<boost::uuids::uuid>
	{
		static const bool primitive = false;
		static const std::uint32_t version = 0;
	};

	template <class Archive>
	void serialize(Archive& archive, boost::uuids::uuid& value, std::uint32_t version)
	{
		for (auto& v : value)
			archive(v);
	}

}
