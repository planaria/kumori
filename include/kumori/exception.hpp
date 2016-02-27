#pragma once

namespace kumori
{

	class exception
		: public std::exception
		, public boost::exception
	{
	public:

		explicit exception(std::string message)
			: message_(message)
		{
		}

		virtual const char* what() const noexcept override
		{
			return message_.c_str();
		}

	private:

		std::string message_;

	};

}
