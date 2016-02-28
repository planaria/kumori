#pragma once

namespace kumori
{

	struct enum_hash
	{

		template <class T>
		std::size_t operator ()(T t) const
		{
			return static_cast<std::size_t>(t);
		}

	};
	
}
