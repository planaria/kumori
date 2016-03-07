#pragma once

namespace kumori
{

	class log_initializer : boost::noncopyable
	{
	public:

		log_initializer()
		{
			std::locale locale = boost::locale::generator().generate("C.UTF-8");

			std::locale::global(locale);
			std::setlocale(LC_ALL, "C");

			boost::filesystem::path::imbue(locale);

			std::cout.imbue(std::locale::classic());
			std::cerr.imbue(std::locale::classic());
			std::clog.imbue(std::locale::classic());

			std::ios::sync_with_stdio(false);

			auto backend = boost::make_shared<backend_type>();
			backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
			backend->auto_flush(true);

			sink_ = boost::make_shared<sink_type>(backend);

			sink_->set_formatter(
				boost::log::expressions::format("%1% [%2%] %3%")
				% boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
				% boost::log::trivial::severity
				% boost::log::expressions::message);

			boost::log::core::get()->add_sink(sink_);

			boost::log::add_common_attributes();
		}

		~log_initializer()
		{
			try
			{
				boost::shared_ptr<boost::log::core> core = boost::log::core::get();
				core->remove_sink(sink_);

				sink_->stop();
				sink_->flush();
			}
			catch (...)
			{
				BOOST_ASSERT(false);
			}
		}

	private:

		typedef boost::log::sinks::text_ostream_backend backend_type;
		typedef boost::log::sinks::asynchronous_sink<backend_type> sink_type;

		boost::shared_ptr<sink_type> sink_;

	};

}
