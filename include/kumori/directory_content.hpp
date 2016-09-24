#pragma once
#include "content.hpp"
#include "regex_content.hpp"
#include "regex_content_proxy.hpp"
#include "moved_content.hpp"
#include "directory_config.hpp"
#include "http_exception.hpp"
#include "path_exception.hpp"
#include "wildcard.hpp"

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
			if (!boost::starts_with(path, "/"))
				BOOST_THROW_EXCEPTION(invalid_path_exception(path));

			if (!content_map_.insert(std::make_pair(path, content)).second)
				BOOST_THROW_EXCEPTION(duplicated_path_exception(path));
		}

		void add(const std::string& path, regex_content* content)
		{
			if (!boost::starts_with(path, "/"))
				BOOST_THROW_EXCEPTION(invalid_path_exception(path));

			regex_content_map_[path].push_back(content);
		}

		template <class Content>
		void add(const std::string& path, const std::shared_ptr<Content>& content)
		{
			if (!boost::starts_with(path, "/"))
				BOOST_THROW_EXCEPTION(invalid_path_exception(path));

			managed_contents_.push_back(content);

			try
			{
				add(path, content.get());
			}
			catch (...)
			{
				managed_contents_.erase(std::find(managed_contents_.begin(), managed_contents_.end(), content));
				throw;
			}
		}

		std::shared_ptr<content> load_file(const std::string& path, const boost::filesystem::path& file, const directory_config& config = directory_config())
		{
			if (!boost::starts_with(path, "/"))
				BOOST_THROW_EXCEPTION(invalid_path_exception(path));

			auto it = config.extension_mime_types.find(file.extension().string());

			boost::filesystem::ifstream stream(file, std::ios_base::binary);
			std::istreambuf_iterator<char> stream_begin(stream);
			std::istreambuf_iterator<char> stream_end;

			std::string content_str(stream_begin, stream_end);
			std::string mime_type = it != config.extension_mime_types.end() ? it->second : config.unknown_mime_type;

			auto conetnt = std::make_shared<static_content>(std::move(content_str), std::move(mime_type));
			add(path, conetnt);

			return conetnt;
		}

		void load_directory(const std::string& path, const boost::filesystem::path& directory, const directory_config& config = directory_config())
		{
			if (!boost::starts_with(path, "/"))
				BOOST_THROW_EXCEPTION(invalid_path_exception(path));

			if (!boost::ends_with(path, "/"))
				BOOST_THROW_EXCEPTION(invalid_path_exception(path));

			BOOST_ASSERT(boost::filesystem::is_directory(directory));

			std::shared_ptr<content> index_content;
			std::size_t index_highest_priority;

			boost::filesystem::directory_iterator directory_begin(directory);
			boost::filesystem::directory_iterator directory_end;

			for (auto it = directory_begin; it != directory_end; ++it)
			{
				boost::filesystem::path filename = it->path().filename();
				boost::filesystem::path nodename = filename;

				if (boost::filesystem::is_directory(it->path()))
				{
					load_directory(path + nodename.string() + '/', it->path(), config);
				}
				else
				{
					if (match_wildcard(filename.string(), config.omit_extensions.begin(), config.omit_extensions.end(), false))
						nodename.replace_extension();

					if (!match_wildcard(filename.string(), config.public_files.begin(), config.public_files.end(), false))
						continue;

					auto content = load_file(path + nodename.string(), it->path(), config);

					auto it = std::find(config.index_files.begin(), config.index_files.end(), filename.string());
					if (it != config.index_files.end())
					{
						std::size_t priority = it - config.index_files.begin();
						if (!index_content || priority < index_highest_priority)
						{
							index_content = content;
							index_highest_priority = priority;
						}
					}
				}
			}

			if (index_content)
			{
				add(path, index_content.get());

				if (path.size() != 1)
				{
					std::string path_without_slash = path;
					path_without_slash.pop_back();

					add(path_without_slash, std::make_shared<moved_content>(path));
				}
			}
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

		std::vector<std::shared_ptr<void>> managed_contents_;

	};

}
