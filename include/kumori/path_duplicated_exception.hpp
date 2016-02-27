#pragma once
#include "exception.hpp"

namespace kumori
{

	class path_duplicated_exception : public exception
	{
	public:

		explicit path_duplicated_exception(std::string path)
			: exception("path_duplicated(" + std::move(path) + ")")
		{
		}

	};

}
