#pragma once

namespace kumori
{

	inline const boost::posix_time::ptime& epoch()
	{
		static boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
		return epoch;
	}

	inline boost::posix_time::ptime to_posix_time(const std::string& str)
	{
		static std::locale locale(std::locale::classic(), new boost::posix_time::time_input_facet("%a, %d %b %Y %H:%M:%S GMT"));
		std::istringstream stream(str);
		stream.imbue(locale);
		boost::posix_time::ptime time;
		stream >> time;
		return time;
	}

	inline std::string format_rfc1123(const boost::posix_time::ptime& time)
	{
		static std::locale locale(std::locale(), new boost::posix_time::time_facet("%a, %d %b %Y %H:%M:%S GMT"));
		std::ostringstream stream;
		stream.imbue(locale);
		stream << time;
		return stream.str();
	}

}
