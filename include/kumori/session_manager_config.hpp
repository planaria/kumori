#pragma once

namespace kumori
{

	struct session_manager_config
	{
		boost::posix_time::time_duration session_life_time = boost::posix_time::hours(8760);
	};

}
