#pragma once
#include "string_util.hpp"
#include "stream_exception.hpp"

namespace kumori
{

	class chunked_decoder
	{
	public:

		typedef char char_type;

		struct category
			: boost::iostreams::input_filter_tag
			, boost::iostreams::multichar_tag
		{
		};

		explicit chunked_decoder(std::size_t maximum_chunk_size)
			: maximum_chunk_size_(maximum_chunk_size)
		{
		}

		template <class Source>
		std::streamsize read(Source& source, char_type* s, std::streamsize n)
		{
			if (!remain_)
			{
				do
				{
					int c = boost::iostreams::get(source);

					if (c == EOF)
						BOOST_THROW_EXCEPTION(stream_exception());

					if (c == boost::iostreams::WOULD_BLOCK)
						return 0;

					if(buffer_.size() >= maximum_chunk_size_)
						BOOST_THROW_EXCEPTION(stream_exception());

					buffer_.push_back(c);
				} while (!boost::algorithm::ends_with(buffer_, "\r\n"));

				buffer_.resize(buffer_.size() - 2);

				std::size_t size;
				if (!to_integer(buffer_, size, 16))
					BOOST_THROW_EXCEPTION(stream_exception());

				remain_ = size;
				buffer_.clear();
			}
			else if (*remain_ == 0)
			{
				while (true)
				{
					if (eof_ == 2)
						return -1;

					int c = boost::iostreams::get(source);
					if (c == EOF)
						BOOST_THROW_EXCEPTION(stream_exception());
					if (c == boost::iostreams::WOULD_BLOCK)
						return 0;

					++eof_;

					if (eof_ == 1)
					{
						if (c != '\r')
							BOOST_THROW_EXCEPTION(stream_exception());
					}
					else if (eof_ == 2)
					{
						if (c != '\n')
							BOOST_THROW_EXCEPTION(stream_exception());
						return -1;
					}
				}
			}

			if (*remain_ < static_cast<std::size_t>(n))
				n = *remain_;

			n = io::read(source, s, n);
			if (n == -1)
				BOOST_THROW_EXCEPTION(stream_exception());

			*remain_ -= n;

			return n;
		}

	private:

		std::size_t maximum_chunk_size_;

		std::string buffer_;
		boost::optional<std::size_t> remain_;
		unsigned char eof_ = 0;

	};

}
