#pragma once
#include "exception.hpp"

namespace kumori
{

	class interrupted_exception : public exception
	{
	public:

		interrupted_exception()
			: exception("interrupted_exception")
		{
		}

	};

}
