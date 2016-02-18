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

		template <class Source>
		std::streamsize read(Source& source, char_type* s, std::streamsize n)
		{
			if (state_ == state::cr_after_chunk)
			{
				int c = boost::iostreams::get(source);
				if (c == EOF)
					BOOST_THROW_EXCEPTION(stream_exception());
				if (c == boost::iostreams::WOULD_BLOCK)
					return 0;

				if (c != '\r')
					BOOST_THROW_EXCEPTION(stream_exception());

				state_ = state::lf_after_chunk;
			}

			if (state_ == state::lf_after_chunk)
			{
				int c = boost::iostreams::get(source);
				if (c == EOF)
					BOOST_THROW_EXCEPTION(stream_exception());
				if (c == boost::iostreams::WOULD_BLOCK)
					return 0;

				if (c != '\n')
					BOOST_THROW_EXCEPTION(stream_exception());

				if (eof_)
					return -1;

				state_ = state::size;
			}

			if (state_ == state::size)
			{
				do
				{
					int c = boost::iostreams::get(source);

					if (c == EOF)
						BOOST_THROW_EXCEPTION(stream_exception());

					if (c == boost::iostreams::WOULD_BLOCK)
						return 0;

					if (buffer_.size() >= 10)
						BOOST_THROW_EXCEPTION(stream_exception());

					buffer_.push_back(c);
				} while (!boost::algorithm::ends_with(buffer_, "\r\n"));

				buffer_.resize(buffer_.size() - 2);

				std::size_t size;
				if (!to_integer(buffer_, size, 16))
					BOOST_THROW_EXCEPTION(stream_exception());

				remain_ = size;
				buffer_.clear();

				state_ = state::chunk;

				if (size == 0)
					eof_ = true;
			}

			BOOST_ASSERT(state_ == state::chunk);

			if (remain_ < static_cast<std::size_t>(n))
				n = remain_;

			n = boost::iostreams::read(source, s, n);
			if (n == -1)
				BOOST_THROW_EXCEPTION(stream_exception());

			remain_ -= n;

			if (remain_ == 0)
				state_ = state::cr_after_chunk;

			return n;
		}

	private:

		enum class state
		{
			size,
			chunk,
			cr_after_chunk,
			lf_after_chunk,
		};

		state state_ = state::size;
		std::size_t remain_ = 0;
		std::string buffer_;
		bool eof_ = false;

	};

}
