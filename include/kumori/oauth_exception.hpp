#pragma once
#include "exception.hpp"

namespace kumori
{

	class oauth_exception : public exception
	{
	public:

		oauth_exception()
			: exception("oauth")
		{
		}

	};

}
