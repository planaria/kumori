#pragma once
#include "http_request_response.hpp"
#include "http_status_code.hpp"
#include "http_entity_header.hpp"
#include "http_content_encoding.hpp"

namespace kumori
{

	class http_response : public http_request_response
	{
	public:

		http_status_code get_status_code() const
		{
			return status_code_;
		}

		void set_status_code(http_status_code value)
		{
			status_code_ = value;
		}

		const std::string& get_location() const
		{
			return location_;
		}

		void set_location(std::string location)
		{
			location_ = std::move(location);
		}

		bool has_vary() const
		{
			return vary_.any();
		}

		bool test_vary(http_entity_header key) const
		{
			BOOST_ASSERT(key < http_entity_header::end);
			return vary_.test(static_cast<std::size_t>(key));
		}

		void set_vary(http_entity_header key, bool value)
		{
			BOOST_ASSERT(key < http_entity_header::end);
			vary_.set(static_cast<std::size_t>(key), value);
		}

		void reset_vary()
		{
			vary_.reset();
		}

		const std::string& get_mime_type() const
		{
			return mime_type_;
		}

		void set_mime_type(std::string value)
		{
			mime_type_ = std::move(value);
		}

		std::size_t get_keep_alive_requests() const
		{
			return keep_alive_requests_;
		}

		void set_keep_alive_requests(std::size_t value)
		{
			keep_alive_requests_ = value;
		}

		const std::string& get_e_tag() const
		{
			return e_tag_;
		}

		void set_e_tag(std::string value)
		{
			e_tag_ = std::move(value);
		}

		const std::string& get_last_modified() const
		{
			return last_modified_;
		}

		void set_last_modified(std::string value)
		{
			last_modified_ = std::move(value);
		}

		http_content_encoding get_content_encoding() const
		{
			return content_encoding_;
		}

		void set_content_encoding(http_content_encoding value)
		{
			content_encoding_ = value;
		}

	private:

		http_status_code status_code_ = http_status_code::ok;
		std::string location_;
		std::bitset<static_cast<std::size_t>(http_entity_header::end)> vary_;
		std::string mime_type_;
		std::size_t keep_alive_requests_ = 0;
		std::string e_tag_;
		std::string last_modified_;
		http_content_encoding content_encoding_ = http_content_encoding::identity;

	};

}
