#pragma once
#include "http_config.hpp"
#include "client_config.hpp"

namespace kumori
{

	struct http_client_config
		: public http_config
		, public client_config
	{
	};

}
