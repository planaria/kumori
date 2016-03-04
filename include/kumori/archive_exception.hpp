#pragma once
#include "exception.hpp"

namespace kumori
{

	class archive_exception : public exception
	{
	public:

		archive_exception()
			: exception("archive")
		{
		}

	};

}
