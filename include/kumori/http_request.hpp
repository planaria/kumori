#pragma once
#include "http_request_response.hpp"
#include "http_method.hpp"
#include "http_content_encoding.hpp"

namespace kumori
{

	class http_request : public http_request_response
	{
	public:

		typedef std::unordered_map<std::string, std::string> parameters_type;

		http_method get_method() const
		{
			return method_;
		}

		void set_method(http_method value)
		{
			method_ = value;
		}

		const std::string& get_host() const
		{
			return host_;
		}

		void set_host(std::string value)
		{
			host_ = std::move(value);
		}

		const std::string& get_path() const
		{
			return path_;
		}

		void set_path(std::string value)
		{
			path_ = std::move(value);
		}

		const parameters_type& parameters() const
		{
			return parameters_;
		}

		parameters_type& parameters()
		{
			return parameters_;
		}

		const std::string& get_authorization() const
		{
			return authorization_;
		}

		void set_authorization(std::string value)
		{
			authorization_ = std::move(value);
		}

		bool get_expect_continue() const
		{
			return expect_continue_;
		}

		void set_expect_continue(bool value)
		{
			expect_continue_ = value;
		}

		bool get_unknown_expectation() const
		{
			return unknown_expectation_;
		}

		void set_unknown_expectation(bool value)
		{
			unknown_expectation_ = value;
		}

		const std::string& get_cached_e_tag() const
		{
			return cached_e_tag_;
		}

		void set_cached_e_tag(std::string value)
		{
			cached_e_tag_ = std::move(value);
		}

		const std::string& get_cached_date() const
		{
			return cached_date_;
		}

		void set_cached_date(std::string value)
		{
			cached_date_ = std::move(value);
		}

		bool test_accept_encoding(http_content_encoding key) const
		{
			BOOST_ASSERT(key < http_content_encoding::end);
			return accept_encoding_.test(static_cast<std::size_t>(key));
		}

		void set_accept_encoding(http_content_encoding key, bool value = true)
		{
			BOOST_ASSERT(key < http_content_encoding::end);
			accept_encoding_.set(static_cast<std::size_t>(key), value);
		}

		void reset_accept_encoding()
		{
			accept_encoding_.reset();
		}

		const std::string& get_csrf_token() const
		{
			return csrf_token_;
		}

		void set_csrf_token(std::string value)
		{
			csrf_token_ = std::move(value);
		}

	private:

		http_method method_ = http_method::end;
		std::string host_;
		std::string path_;
		parameters_type parameters_;
		std::string authorization_;
		bool expect_continue_ = false;
		bool unknown_expectation_ = false;
		std::string cached_e_tag_;
		std::string cached_date_;
		std::bitset<static_cast<std::size_t>(http_content_encoding::end)> accept_encoding_;
		std::string csrf_token_;
		std::string token_;

	};

}
