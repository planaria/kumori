#pragma once
#include "interrupted_exception.hpp"

namespace kumori
{

	class sync : boost::noncopyable
	{
	public:

		sync(boost::asio::io_service& service, const std::function<void()>& entry_point, std::size_t stack_size)
			: entry_point_(entry_point)
			, timer_(service)
			, stack_(stack_size)
			, context_(std::allocator_arg_t(), stack_, &sync::callback)
		{
		}

		~sync()
		{
		}

		void resume()
		{
			std::lock_guard<std::mutex> lock(mutex_);
			current_impl() = this;
			context_ = std::get<0>(context_(this));
		}

		void suspend()
		{
			current_impl() = nullptr;
			BOOST_ASSERT(!mutex_.try_lock());
			BOOST_ASSERT(original_context_);
			original_context_ = std::get<0>(original_context_(nullptr));
		}

		template <class Callback>
		void suspend_for(const boost::posix_time::time_duration& timeout, Callback&& cancel)
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

		static boost::context::execution_context<sync*> callback(boost::context::execution_context<sync*> context, sync* s)
		{
			return s->run(std::move(context));
		}

		boost::context::execution_context<sync*> run(boost::context::execution_context<sync*> context)
		{
			original_context_ = std::move(context);

			try
			{
				entry_point_();
			}
			catch (boost::context::detail::forced_unwind&)
			{
				throw;
			}
			catch (...)
			{
			}

			return std::move(original_context_);
		}

		static sync*& current_impl()
		{
			thread_local sync* s = nullptr;
			return s;
		}

		std::function<void()> entry_point_;

		std::mutex mutex_;

		boost::asio::deadline_timer timer_;
		std::atomic<bool> interrupted_{ false };

		boost::context::protected_fixedsize_stack stack_;

		boost::context::execution_context<sync*> context_;
		boost::context::execution_context<sync*> original_context_;

	};

}
