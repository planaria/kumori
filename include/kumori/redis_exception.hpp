#pragma once
#include "exception.hpp"

namespace kumori
{

	class redis_exception : public exception
	{
	public:

		explicit redis_exception(std::string message)
			: exception("redis(" + std::move(message) + ")")
		{
		}

	};

}
