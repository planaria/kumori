#pragma once
#include "exception.hpp"

namespace kumori
{

	class http_parse_exception : public exception
	{
	public:

		http_parse_exception()
			: exception("http_parse_exception")
		{
		}

	};

}
