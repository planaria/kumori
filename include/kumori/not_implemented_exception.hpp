#pragma once
#include "exception.hpp"

namespace kumori
{

	class not_implemented_exception : public exception
	{
	public:

		virtual const char* what() const override
		{
			return "not_implemented";
		}

	};

}
