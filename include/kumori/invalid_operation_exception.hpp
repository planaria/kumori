#pragma once
#include "exception.hpp"

namespace kumori
{

	class invalid_operation_exception : public exception
	{
	public:

		invalid_operation_exception()
			: exception("invalid_operation_exception")
		{
		}

	};

}
