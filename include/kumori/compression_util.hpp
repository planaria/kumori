#pragma once

namespace kumori
{

	inline std::string compress_gzip(const std::string& str)
	{
		boost::iostreams::gzip_params params;
		params.level = boost::iostreams::gzip::best_compression;

		std::stringstream result;
		boost::iostreams::filtering_ostream stream;
		stream.push(boost::iostreams::gzip_compressor());
		stream.push(result);
		stream << str;
		boost::iostreams::close(stream);
		return result.str();
	}

	inline std::string compress_deflate(const std::string& str)
	{
		boost::iostreams::zlib_params params;
		params.level = boost::iostreams::zlib::best_compression;
		params.noheader = true;

		std::stringstream result;
		boost::iostreams::filtering_ostream stream;
		stream.push(boost::iostreams::zlib_compressor(params));
		stream.push(result);
		stream << str;
		boost::iostreams::close(stream);
		return result.str();
	}

}
