#pragma once
#include "content.hpp"
#include "compression_util.hpp"
#include "hash_util.hpp"
#include "base64_util.hpp"
#include "date_time_util.hpp"

namespace kumori
{

	class static_content : public content
	{
	public:

		static_content()
		{
		}

		static_content(std::string content, std::string mime_type)
		{
			set_mime_type(std::move(mime_type));
			set_content(std::move(content));
		}

		virtual void on_get(http_server_context& context, bool head) override
		{
			http_request& req = context.request();
			http_response& res = context.response();

			if (encoded_contents_.empty())
			{
				context.response().set_content_length(0);
				context.write_headers();
				return;
			}

			if (req.get_cached_e_tag() == e_tag_ || req.get_cached_date() == last_modified_)
			{
				res.set_status_code(http_status_code::not_modified);
				res.set_content_length(0);
				context.write_headers();
				return;
			}

			auto it = std::find_if(encoded_contents_.begin(), encoded_contents_.end(),
				[&](const encoded_content& content)
			{
				if (content.first == http_content_encoding::identity)
					return true;
				return req.test_accept_encoding(content.first);
			});

			BOOST_ASSERT(it != encoded_contents_.end());

			res.set_vary(http_entity_header::accept_encoding, true);
			res.set_content_encoding(it->first);
			res.set_content_length(it->second.size());
			res.set_mime_type(mime_type_);
			res.set_e_tag(e_tag_);
			res.set_last_modified(last_modified_);
			context.write_headers();

			if (head)
				return;

			output(it->second, context.response_stream());
		}

		void set_mime_type(std::string mime_type)
		{
			mime_type_ = std::move(mime_type);
		}

		void set_content(std::string content)
		{
			std::string content_gzip = compress_gzip(content);
			std::string content_deflate = compress_deflate(content);

			std::string e_tag = encode_base64(hash_md5(content));
			std::string last_modified = format_rfc1123(boost::posix_time::second_clock::universal_time());

			std::vector<encoded_content> encoded_contents;

			if (content_gzip.size() < content.size())
				encoded_contents.push_back(std::make_pair(http_content_encoding::gzip, std::move(content_gzip)));

			if (content_deflate.size() < content.size())
				encoded_contents.push_back(std::make_pair(http_content_encoding::deflate, std::move(content_deflate)));

			std::sort(encoded_contents.begin(), encoded_contents.end(),
				[](const encoded_content& lhs, const encoded_content& rhs)
			{
				return lhs.second.size() < rhs.second.size();
			});

			encoded_contents.push_back(std::make_pair(http_content_encoding::identity, std::move(content)));

			encoded_contents_.swap(encoded_contents);
			e_tag_.swap(e_tag);
			last_modified_.swap(last_modified);
		}

	private:

		std::string mime_type_;

		typedef std::pair<http_content_encoding, std::string> encoded_content;
		std::vector<encoded_content> encoded_contents_;

		std::string e_tag_;
		std::string last_modified_;

	};

}
