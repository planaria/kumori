#pragma once

namespace kumori
{

	template <class Archive, class T>
	void serialize_split(Archive& archive, T& value, std::uint32_t version,
		typename std::enable_if<Archive::mode == archive_mode::input>::type* = 0)
	{
		load(archive, value, version);
	}

	template <class Archive, class T>
	void serialize_split(Archive& archive, T& value, std::uint32_t version,
		typename std::enable_if<Archive::mode == archive_mode::output>::type* = 0)
	{
		save(archive, value, version);
	}

}
