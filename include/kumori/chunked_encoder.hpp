#pragma once
#include "string_util.hpp"
#include "stream_exception.hpp"

namespace kumori
{

	class chunked_encoder
	{
	public:

		typedef char char_type;

		struct category
			: boost::iostreams::output_filter_tag
			, boost::iostreams::closable_tag
			, boost::iostreams::multichar_tag
		{
		};

		template <class Synk>
		std::streamsize write(Synk& synk, const char_type* s, std::streamsize n)
		{
			std::string size_str = to_hex(static_cast<boost::make_unsigned<std::streamsize>::type>(n));
			if (boost::iostreams::write(synk, size_str.c_str(), size_str.size()) != static_cast<std::streamsize>(size_str.size()))
				BOOST_THROW_EXCEPTION(stream_exception());
			if (boost::iostreams::write(synk, "\r\n", 2) != 2)
				BOOST_THROW_EXCEPTION(stream_exception());
			if (boost::iostreams::write(synk, s, n) != n)
				BOOST_THROW_EXCEPTION(stream_exception());
			if (boost::iostreams::write(synk, "\r\n", 2) != 2)
				BOOST_THROW_EXCEPTION(stream_exception());
			return n;
		}

		template <class Synk>
		void close(Synk& synk)
		{
			if (boost::iostreams::write(synk, "0\r\n\r\n", 5) != 5)
				BOOST_THROW_EXCEPTION(stream_exception());
		}

	};

}
