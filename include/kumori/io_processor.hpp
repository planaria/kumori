#pragma once

namespace kumori
{

	class io_processor
	{
	public:

		io_processor(boost::asio::io_service& service, std::size_t num_threads)
			: service_(service)
		{
			threads_.reserve(num_threads);

			for (std::size_t i = 0; i < num_threads; ++i)
				threads_.push_back(std::thread(std::bind(&io_processor::run, this)));
		}

		~io_processor()
		{
			try
			{
				join();
			}
			catch (...)
			{
			}
		}

		void join()
		{
			for (std::thread& thread : threads_)
				thread.join();

			threads_.clear();
		}

		boost::signals2::connection hook_exception(const std::function<void ()>& callback)
		{
			return on_exception_.connect(callback);
		}

	private:

		void run()
		{
			while (true)
			{
				try
				{
					service_.run();
					break;
				}
				catch (...)
				{
					on_exception_();
				}
			}
		}

		boost::asio::io_service& service_;

		std::vector<std::thread> threads_;

		boost::signals2::signal<void()> on_exception_;

	};

}
