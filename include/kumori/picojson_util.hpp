#pragma once

namespace kumori
{

	inline std::string get_or_empty(const picojson::object& tree, const std::string& key)
	{
		auto it = tree.find(key);
		if (it == tree.end())
			return std::string();

		return it->second.to_str();
	}

}
