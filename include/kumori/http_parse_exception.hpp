#pragma once
#include "exception.hpp"

namespace kumori
{

	class http_parse_exception : public exception
	{
	public:

		virtual const char* what() const throw() override
		{
			return "http_parse";
		}

	};

}
