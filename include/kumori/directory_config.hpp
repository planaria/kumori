#pragma once

namespace kumori
{

	struct directory_config
	{

		std::vector<std::string> index_files
		{
			"index.html",
			"index.htm",
		};

		std::string unknown_mime_type = "application/octet-stream";

		std::unordered_map<std::string, std::string> extension_mime_types
		{
			{ ".html", "text/html" },
			{ ".htm", "text/html" },
			{ ".png", "image/png" },
			{ ".jpeg", "image/jpeg" },
			{ ".jpg", "image/jpeg" },
			{ ".ico", "image/x-icon" },
			{ ".css", "text/css" },
			{ ".js", "application/javascript" },
			{ ".map", "application/json" },
			{ ".txt", "text/plain" },
		};

		std::unordered_set<std::string> omit_extensions
		{
			".html",
			".htm",
		};

	};

}
