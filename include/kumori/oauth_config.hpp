#pragma once
#include "url.hpp"
#include "redis_client_config.hpp"

namespace kumori
{

	struct oauth_config
	{
		std::string site_name;
		url authorize_url;
		url access_token_url;
		url callback_url;
		std::string certificate;
	};

}
