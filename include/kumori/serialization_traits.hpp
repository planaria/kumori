#pragma once

namespace kumori
{

	template <class T, class Enable = void>
	struct serialization_traits
	{
		static const bool primitive = false;
		static const std::uint32_t version = T::serialize_version;
	};

	template <class T>
	struct serialization_traits<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
	{
		static const bool primitive = true;
	};

	template <>
	struct serialization_traits<std::string>
	{
		static const bool primitive = true;
	};

	template <>
	struct serialization_traits<const char*>
	{
		static const bool primitive = true;
	};

	template <std::size_t N>
	struct serialization_traits<char [N]>
	{
		static const bool primitive = true;
	};

}
