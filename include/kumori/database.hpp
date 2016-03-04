#pragma once
#include "database_connection.hpp"

namespace kumori
{

	class database : boost::noncopyable
	{
	public:

		virtual ~database()
		{
		}

		virtual void connect(const std::function<void (database_connection&)>& callback) = 0;

	};

}
