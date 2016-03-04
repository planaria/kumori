#pragma once
#include "redis_client_config.hpp"

namespace kumori
{

	struct redis_database_config
	{
		redis_client_config client;
	};

}
