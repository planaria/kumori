#pragma once
#include "sync.hpp"
#include "async_mutex.hpp"

namespace kumori
{

	class sync_lock : boost::noncopyable
	{
	public:

		explicit sync_lock(async_mutex& mutex)
			: mutex_(mutex)
		{
			sync& sync = sync::current();

			mutex_.async_lock([&]()
			{
				sync.resume();
			});

			sync.suspend();
		}

		~sync_lock()
		{
			mutex_.unlock();
		}

	private:

		async_mutex& mutex_;

	};

}
