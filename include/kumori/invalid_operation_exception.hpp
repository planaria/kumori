#pragma once
#include "exception.hpp"

namespace kumori
{

	class invalid_operation_exception : public exception
	{
	public:

		virtual const char* what() const override
		{
			return "invalid_operation_exception";
		}

	};

}
