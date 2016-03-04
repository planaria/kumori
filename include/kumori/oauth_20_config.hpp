#pragma once
#include "oauth_config.hpp"

namespace kumori
{

	struct oauth_20_config : oauth_config
	{
		std::string client_id;
		std::string client_secret;
	};

}
