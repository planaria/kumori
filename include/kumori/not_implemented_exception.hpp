#pragma once
#include "exception.hpp"

namespace kumori
{

	class not_implemented_exception : public exception
	{
	public:

		not_implemented_exception()
			: exception("not_implemented_exception")
		{
		}

	};

}
