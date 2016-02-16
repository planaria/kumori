#pragma once
#include "exception.hpp"
#include "http_status_code.hpp"

namespace kumori
{

	class http_exception : public exception
	{
	public:

		explicit http_exception(http_status_code status_code)
			: status_code_(status_code)
		{
		}

		virtual const char* what() const override
		{
			return "http";
		}

		http_status_code status_code() const
		{
			return status_code_;
		}

	private:

		http_status_code status_code_;

	};

}
