#pragma once
#include "async_mutex_service.hpp"

namespace kumori
{

	class async_mutex : public boost::asio::basic_io_object<async_mutex_service>
	{
	public:

		explicit async_mutex(boost::asio::io_service& service)
			: boost::asio::basic_io_object<async_mutex_service>(service)
		{
		}

		template <class Callback>
		void async_lock(Callback&& callback)
		{
			this->get_service().async_lock(this->get_implementation(), std::forward<Callback>(callback));
		}

		void unlock()
		{
			this->get_service().unlock(this->get_implementation());
		}

	};

}
