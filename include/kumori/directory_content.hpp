#pragma once
#include "content.hpp"
#include "regex_content.hpp"
#include "regex_content_proxy.hpp"
#include "http_exception.hpp"
#include "path_duplicated_exception.hpp"

namespace kumori
{

	class directory_content : public content
	{
	public:

		virtual void on_get(http_server_context& context, bool head) override
		{
			get_content(context.request().get_path(),
				[&](content* content)
			{
				if (!content)
					BOOST_THROW_EXCEPTION(http_exception(http_status_code::not_found));

				content->on_get(context, head);
			});
		}

		virtual void on_post(http_server_context& context) override
		{
			get_content(context.request().get_path(),
				[&](content* content)
			{
				if (!content)
					BOOST_THROW_EXCEPTION(http_exception(http_status_code::not_found));

				content->on_post(context);
			});
		}

		void add(const std::string& path, content* content)
		{
			if (!content_map_.insert(std::make_pair(path, content)).second)
				BOOST_THROW_EXCEPTION(path_duplicated_exception(path));
		}

		void add(const std::string& path, regex_content* content)
		{
			regex_content_map_[path].push_back(content);
		}

	private:

		template <class Callback>
		void get_content(const std::string& path, Callback&& callback)
		{
			{
				auto it = content_map_.find(path);
				if (it != content_map_.end())
				{
					callback(it->second);
					return;
				}
			}

			{
				auto range = regex_content_map_.longest_match(path);

				if (range.first != range.second && range.first->first.size() <= path.size())
				{
					if (std::equal(range.first->first.begin(), range.first->first.end(), path.begin()))
					{
						auto begin = path.begin() + range.first->first.size();

						std::smatch match;

						for (regex_content* content : range.first->second)
						{
							if (!std::regex_match(begin, path.end(), match, content->pattern()))
								continue;

							regex_content_proxy proxy(*content, match);
							callback(&proxy);
							return;
						}
					}
				}
			}

			callback(nullptr);
		}

		cb_tree::cb_map<std::string, content*> content_map_;
		cb_tree::cb_map<std::string, std::vector<regex_content*>> regex_content_map_;

	};

}
