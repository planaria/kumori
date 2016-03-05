#pragma once

namespace kumori
{

	class async_mutex_service : public boost::asio::detail::service_base<async_mutex_service>
	{
	public:

		struct implementation_type : boost::noncopyable
		{
		};

		explicit async_mutex_service(boost::asio::io_service& service)
			: boost::asio::detail::service_base<async_mutex_service>(service)
			, thread_(std::bind(&async_mutex_service::run, this))
		{
		}

		void construct(implementation_type& /*impl*/)
		{
		}

		void destroy(implementation_type& /*impl*/)
		{
		}

		template <class Callback>
		void async_lock(implementation_type& impl, Callback callback)
		{
			{
				std::lock_guard<std::mutex> lock(mutex_);

				queue_.push_back(std::make_unique<lock_operation<Callback>>(this->get_io_service(), impl, std::move(callback)));
			}

			cv_.notify_one();
		}

		void unlock(implementation_type& /*impl*/)
		{
			{
				std::lock_guard<std::mutex> lock(mutex_);
				BOOST_ASSERT(!unlock_);
				unlock_ = true;
			}

			cv_.notify_one();
		}

	private:

		class lock_operation_base
		{
		public:

			virtual ~lock_operation_base()
			{
			}

			virtual void invoke() = 0;

		};

		template <class Callback>
		class lock_operation : public lock_operation_base
		{
		public:

			lock_operation(boost::asio::io_service& service, implementation_type& impl, Callback&& callback)
				: service_(service)
				, impl_(impl)
				, callback_(std::move(callback))
			{
			}

			virtual void invoke() override
			{
				service_.post(callback_);
			}

		private:

			boost::asio::io_service& service_;
			implementation_type& impl_;
			Callback callback_;

		};

		virtual void shutdown_service() override
		{
			{
				std::unique_lock<std::mutex> lock(mutex_);
				shutdown_ = true;
			}

			cv_.notify_one();

			thread_.join();
		}

		void run()
		{
			while (true)
			{
				std::unique_ptr<lock_operation_base> op;

				{
					std::unique_lock<std::mutex> lock(mutex_);

					cv_.wait(lock, [&] { return shutdown_ || !queue_.empty(); });
					if (shutdown_)
						break;

					op = std::move(queue_.front());
					queue_.pop_front();
				}

				op->invoke();

				{
					std::unique_lock<std::mutex> lock(mutex_);

					cv_.wait(lock, [&] { return shutdown_ || unlock_; });
					if (shutdown_)
						break;

					unlock_ = false;
				}
			}
		}

		std::deque<std::unique_ptr<lock_operation_base>> queue_;
		bool unlock_ = false;
		bool shutdown_ = false;

		std::mutex mutex_;
		std::condition_variable cv_;

		std::thread thread_;

	};

}
