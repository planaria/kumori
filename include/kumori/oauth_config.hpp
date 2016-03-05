#pragma once
#include "url.hpp"
#include "session_manager_config.hpp"

namespace kumori
{

	struct oauth_config
	{
		std::string site_name;
		url authorize_url;
		url access_token_url;
		url callback_url;
		std::string certificate;

		session_manager_config session_manager;
	};

}
