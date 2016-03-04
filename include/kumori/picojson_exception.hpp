#pragma once
#include "exception.hpp"

namespace kumori
{

	class picojson_exception : public exception
	{
	public:

		explicit picojson_exception(std::string message)
			: exception("picojson(" + message + ")")
		{
		}

	};

}

#define CHECK_PICOJSON(x) \
	do\
	{\
		std::string error__ = (x);\
		if(!error__.empty())\
			BOOST_THROW_EXCEPTION(kumori::picojson_exception(error__));\
	}\
	while(false);
