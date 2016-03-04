#pragma once
#include "protocol.hpp"
#include "url_exception.hpp"
#include "constant_strings.hpp"

namespace kumori
{

	class url
	{
	public:

		url()
		{
		}

		url(const char* str)
			: url(std::string(str))
		{
		}

		url(std::string str)
			: raw_(std::move(str))
		{
			auto protocol_end = std::search(
				raw_.begin(), raw_.end(),
				constant_strings::protocol_end().begin(),
				constant_strings::protocol_end().end());
			if (protocol_end == raw_.end())
				BOOST_THROW_EXCEPTION(url_exception());

			protocol_ = to_protocol(std::string(raw_.begin(), protocol_end));
			if (protocol_ == protocol::end)
				BOOST_THROW_EXCEPTION(url_exception());

			auto host_begin = protocol_end + constant_strings::protocol_end().size();
			auto path_begin = std::find(host_begin, raw_.end(), '/');

			auto host_end = std::find(host_begin, path_begin, ':');

			host_.assign(host_begin, host_end);

			if (host_end != path_begin)
			{
				auto portBegin = boost::next(host_end);
				if (portBegin == path_begin)
					BOOST_THROW_EXCEPTION(url_exception());

				port_ = boost::lexical_cast<unsigned short>(boost::make_iterator_range(portBegin, path_begin));
			}
			else
			{
				switch (protocol_)
				{
				case protocol::http:
					port_ = 80;
					break;
				case protocol::https:
					port_ = 443;
					break;
				default:
					BOOST_THROW_EXCEPTION(url_exception());
				}
			}

			if (path_begin != raw_.end())
				path_.assign(path_begin, raw_.end());
			else
				path_ = "/";
		}

		const std::string& get_raw() const
		{
			return raw_;
		}

		protocol get_protocol() const
		{
			return protocol_;
		}

		const std::string& get_host() const
		{
			return host_;
		}

		unsigned short get_port() const
		{
			return port_;
		}

		const std::string& get_path() const
		{
			return path_;
		}

	private:

		std::string raw_;

		protocol protocol_ = protocol::http;
		std::string host_;
		unsigned short port_ = 80;
		std::string path_;

	};

	inline std::ostream& operator <<(std::ostream& stream, const url& url)
	{
		stream << url.get_raw();
		return stream;
	}

}
