#pragma once
#include "protocol.hpp"

namespace kumori
{

	class uri
	{
	public:

		uri()
		{
		}

		uri(std::string str)
			: raw_(std::move(str))
		{
			auto protocolEnd = boost::search(url, wpConstStrings::ProtocolEnd());
			if (protocolEnd == url.end())
				BOOST_THROW_EXCEPTION(wpUrlParseException());

			m_protocol = ToProtocol(std::string(url.begin(), protocolEnd));
			if (m_protocol == ProtocolEnd)
				BOOST_THROW_EXCEPTION(wpUrlParseException());

			auto hostBegin = protocolEnd + wpConstStrings::ProtocolEnd().size();
			auto pathBegin = std::find(hostBegin, url.end(), '/');

			auto hostEnd = std::find(hostBegin, pathBegin, ':');

			m_host.assign(hostBegin, hostEnd);

			if (hostEnd != pathBegin)
			{
				auto portBegin = boost::next(hostEnd);
				if (portBegin == pathBegin)
					BOOST_THROW_EXCEPTION(wpUrlParseException());
				m_port.assign(portBegin, pathBegin);
			}
			else
			{
				switch (m_protocol)
				{
				case ProtocolHttp:
					m_port = "80";
					break;
				case ProtocolHttps:
					m_port = "443";
					break;
				default:
					BOOST_THROW_EXCEPTION(wpUrlParseException());
				}
			}

			if (pathBegin != url.end())
				m_path.assign(pathBegin, url.end());
			else
				m_path = "/";
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

		const std::string& get_port() const
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
		std::string port_;
		std::string path_;

	};

	inline std::ostream& operator <<(std::ostream& stream, const uri& uri)
	{
		stream << uri.get_raw();
		return stream;
	}

}
