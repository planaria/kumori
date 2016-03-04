#pragma once
#include "serialization.hpp"

namespace kumori
{

	struct session
	{
		static const std::uint32_t serialize_version = 0;

		boost::posix_time::ptime timestamp;
		boost::uuids::uuid csrf_token;

		std::string oauth_site_name;
		std::string oauth_token;
		std::string oauth_token_secret;

		boost::optional<boost::uuids::uuid> user_id;
	};

	template <class Archive>
	void serialize(Archive& archive, session& value, std::uint32_t /*version*/)
	{
		archive(value.timestamp);
		archive(value.csrf_token);
		archive(value.oauth_site_name);
		archive(value.oauth_token);
		archive(value.oauth_token_secret);
		archive(value.user_id);
	}

}
