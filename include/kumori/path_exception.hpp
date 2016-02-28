#pragma once
#include "exception.hpp"

namespace kumori
{

	class duplicated_path_exception : public exception
	{
	public:

		explicit duplicated_path_exception(std::string path)
			: exception("duplicated_path(" + std::move(path) + ")")
		{
		}

	};

	class invalid_path_exception : public exception
	{
	public:

		explicit invalid_path_exception(std::string path)
			: exception("invalid_path(" + std::move(path) + ")")
		{
		}

	};

}
