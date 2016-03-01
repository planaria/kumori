#pragma once

namespace kumori
{

	enum class redis_client_type
	{
		normal,
		master,
		slave,
		pubsub,
	};

	inline std::ostream& operator <<(std::ostream& stream, redis_client_type type)
	{
		switch (type)
		{
		case redis_client_type::normal:
			stream << "TYPE normal";
			break;
		case redis_client_type::master:
			stream << "TYPE master";
			break;
		case redis_client_type::slave:
			stream << "TYPE slave";
			break;
		case redis_client_type::pubsub:
			stream << "TYPE pubsub";
			break;
		default:
			BOOST_ASSERT(false);
		}

		return stream;
	}

}
