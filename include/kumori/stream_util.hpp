#pragma once

namespace kumori
{

	inline std::ostream::pos_type get_size(std::ostream& stream)
	{
		auto original_pos = stream.tellp();
		stream.seekp(0, std::ios::end);
		auto pos = stream.tellp();
		stream.seekp(original_pos);
		return pos;
	}

	inline void output(const std::string& str, std::ostream& stream)
	{
		std::ostreambuf_iterator<char> it(stream);
		std::copy(str.begin(), str.end(), it);
	}

	inline void forward(std::istream& input_stream, std::ostream& output_stream)
	{
		std::istreambuf_iterator<char> input_begin(input_stream);
		std::istreambuf_iterator<char> input_end;
		std::ostreambuf_iterator<char> output_it(output_stream);
		std::copy(input_begin, input_end, output_it);
	}

	inline void forward(std::istream& input_stream, std::ostream& output_stream, std::streamsize size)
	{
		std::istreambuf_iterator<char> input_it(input_stream);
		std::ostreambuf_iterator<char> output_it(output_stream);

		while (size-- > 0)
			*output_it++ = *input_it++;
	}

	inline void ignore_until_end(std::istream& stream)
	{
		stream.ignore(std::numeric_limits<std::streamsize>::max());
	}

	inline std::string read_until_end(std::istream& stream)
	{
		std::string result;

		int c;
		while ((c = stream.get()) != EOF)
			result.push_back(c);

		return result;
	}

}
