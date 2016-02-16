#pragma once
#include "socket.hpp"

namespace kumori
{

	class socket_streambuf
		: public std::streambuf
		, boost::noncopyable
	{
	public:

		socket_streambuf(
			socket& socket,
			const boost::posix_time::time_duration& timeout,
			char* input_buffer,
			std::size_t input_buffer_size,
			char* output_buffer,
			std::size_t output_buffer_size)
			: socket_(socket)
			, timeout_(timeout)
			, input_buffer_size_(input_buffer_size)
		{
			char* input_buffer_end = input_buffer + input_buffer_size;
			setg(input_buffer, input_buffer_end, input_buffer_end);

			char* output_buffer_end = output_buffer + output_buffer_size;
			setp(output_buffer, output_buffer_end);
		}

		virtual int_type underflow() override
		{
			if (gptr() == egptr())
			{
				std::size_t n = socket_.read_some(eback(), input_buffer_size_, timeout_);

				if (n == 0)
					return traits_type::eof();

				setg(eback(), eback(), eback() + n);
			}

			return traits_type::to_int_type(*gptr());
		}

		virtual int_type overflow(int_type char_int = traits_type::eof()) override
		{
			std::size_t size = pptr() - pbase();
			std::size_t offset = 0;

			if (!traits_type::eq_int_type(char_int, traits_type::eof()))
			{
				*pptr() = traits_type::to_char_type(char_int);
				++size;
			}

			while (offset != size)
			{
				char* buffer = pbase() + offset;
				std::size_t buffer_size = size - offset;
				std::size_t length = socket_.write_some(buffer, buffer_size, timeout_);
				offset += length;
			}

			setp(pbase(), epptr());

			return traits_type::not_eof(char_int);
		}

		virtual int sync() override
		{
			bool error = traits_type::eq_int_type(overflow(), traits_type::eof());
			return error ? -1 : 0;
		}

	private:

		socket& socket_;
		boost::posix_time::time_duration timeout_;

		std::size_t input_buffer_size_;

	};

}
