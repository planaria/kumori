#pragma once
#include "exception.hpp"

namespace kumori
{

	class url_exception : public exception
	{
	public:

		url_exception()
			: exception("url")
		{
		}

	};

}
