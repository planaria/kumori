#pragma once
#include "exception.hpp"

namespace kumori
{

	class interrupted_exception : public exception
	{
	public:

		virtual const char* what() const override
		{
			return "interrupted";
		}

	};

}
