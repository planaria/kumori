#pragma once
#include "exception.hpp"

namespace kumori
{

	class csrf_exception : public exception
	{
	public:

		csrf_exception()
			: exception("csrf")
		{
		}

	};

}
