#pragma once
#include "oauth_config.hpp"

namespace kumori
{

	struct oauth_10_config : oauth_config
	{
		std::string consumer_key;
		std::string consumer_secret;
		url request_token_url;
	};

}
