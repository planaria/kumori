#pragma once
#include "exception.hpp"

namespace kumori
{

	class stream_exception : public exception
	{
	public:

		virtual const char* what() const throw() override
		{
			return "stream";
		}

	};

}
