#pragma once
#include "http_status_code.hpp"

namespace kumori
{

	struct http_server_config
	{

		std::size_t maximum_keep_alive_requests = 10;
		boost::posix_time::time_duration keep_alive_timeout = boost::posix_time::seconds(10);

		boost::posix_time::time_duration session_life_time = boost::posix_time::time_duration(8760, 0, 0);

		std::size_t maximum_path_length = 2048;
		std::size_t maximum_header_length = 2048;
		std::streamsize maximum_request_content_length = 102400;

		bool strict_http = true;

		std::unordered_map<http_status_code, std::string> error_document =
		{
			{ http_status_code::moved_permanently, "/error/301" },
			{ http_status_code::found, "/error/302" },
			{ http_status_code::not_modified, "/error/304" },
			{ http_status_code::bad_request, "/error/400" },
			{ http_status_code::unauthorized, "/error/401" },
			{ http_status_code::forbidden, "/error/403" },
			{ http_status_code::not_found, "/error/404" },
			{ http_status_code::method_not_allowed, "/error/405" },
			{ http_status_code::request_entity_too_large, "/error/413" },
			{ http_status_code::request_uri_too_long, "/error/414" },
			{ http_status_code::expectation_failed, "/error/417" },
			{ http_status_code::internal_service_error, "/error/500" },
			{ http_status_code::not_implemented, "/error/501" },
			{ http_status_code::http_version_not_supported, "/error/505" },
		};

	};

}
