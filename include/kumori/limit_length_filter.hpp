#pragma once

namespace kumori
{

	class limit_length_filter
	{
	public:

		typedef char char_type;

		struct category
			: boost::iostreams::input_filter_tag
			, boost::iostreams::multichar_tag
		{
		};

		explicit limit_length_filter(std::size_t length)
			: remain_(length)
		{
		}

		template <class Source>
		std::streamsize read(Source& source, char_type* s, std::streamsize n)
		{
			if (remain_ == 0)
				return -1;

			if (remain_ < static_cast<std::size_t>(n))
				n = remain_;

			n = boost::iostreams::read(source, s, n);
			if (n == -1)
				return -1;

			remain_ -= n;

			return n;
		}

	private:

		std::size_t remain_;

	};

}
