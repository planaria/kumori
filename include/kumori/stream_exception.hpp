#pragma once
#include "exception.hpp"

namespace kumori
{

	class stream_exception : public exception
	{
	public:

		stream_exception()
			: exception("stream_exception")
		{
		}

	};

}
