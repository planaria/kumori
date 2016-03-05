#pragma once
#include "interrupted_exception.hpp"

namespace kumori
{

	class sync : boost::noncopyable
	{
	public:

		sync(boost::asio::io_service& service, const std::function<void()>& entry_point, std::size_t stack_size)
			: entry_point_(entry_point)
			, stack_allocator_(stack_size)
			, timer_(service)
		{
			stack_ = stack_allocator_.allocate();
			context_ = boost::context::make_fcontext(stack_.sp, stack_.size, &sync::callback);
		}

		~sync()
		{
			stack_allocator_.deallocate(stack_);
		}

		void resume()
		{
			std::lock_guard<std::mutex> lock(mutex_);
			current_impl() = this;

			std::intptr_t param = reinterpret_cast<std::intptr_t>(this);
			boost::context::jump_fcontext(&original_context_, context_, param);
		}

		void suspend()
		{
			current_impl() = nullptr;
			BOOST_ASSERT(!mutex_.try_lock());
			boost::context::jump_fcontext(&context_, original_context_, 0);
		}

		template <class Callback>
		void suspend_for(const boost::posix_time::time_duration& timeout, Callback& cancel)
		{
			timer_.expires_from_now(timeout);

			timer_.async_wait(
				[&](const boost::system::error_code&)
			{
				resume();
			});

			suspend();

			timer_.cancel();

			cancel();

			suspend();

			if (interrupted_)
				BOOST_THROW_EXCEPTION(interrupted_exception());
		}

		void interrupt()
		{
			interrupted_ = true;
			timer_.cancel();
		}

		static sync& current()
		{
			BOOST_ASSERT(current_impl());
			return *current_impl();
		}

	private:

		static void callback(std::intptr_t param)
		{
			reinterpret_cast<sync*>(param)->run();
		}

		void run()
		{
			try
			{
				entry_point_();
			}
			catch (...)
			{
			}

			suspend();
		}

		static sync*& current_impl()
		{
			thread_local sync* s = nullptr;
			return s;
		}

		std::function<void()> entry_point_;

		boost::context::protected_fixedsize_stack stack_allocator_;
		boost::context::stack_context stack_;

		boost::context::fcontext_t context_;
		boost::context::fcontext_t original_context_;

		std::mutex mutex_;

		boost::asio::deadline_timer timer_;
		std::atomic<bool> interrupted_{ false };

	};

}
