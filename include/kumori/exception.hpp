#pragma once

namespace kumori
{

	class exception
		: public std::exception
		, public boost::exception
	{
	};

}
