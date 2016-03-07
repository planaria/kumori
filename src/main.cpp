#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <kumori/kumori.hpp>

class application_server : public kumori::http_server<application_server>
{
public:

	explicit application_server(boost::asio::io_service& service)
		: http_server(service)
	{
	}

	void on_get(kumori::http_server_context& context, bool head)
	{
		context.response().set_content_type("text/plain");
		context.write_headers();

		if (head)
			return;

		auto& stream = context.response_stream();
		stream << "Hello, World!" << std::endl;
	}

};

int main(int /*argc*/, char* /*argv*/[])
{
	try
	{
		kumori::log_initializer log;

		boost::asio::io_service service;

		application_server server(service);

		server.start();

		boost::asio::signal_set signals(service);

		signals.add(SIGINT);
		signals.add(SIGTERM);

#ifdef SIGBREAK
		signals.add(SIGBREAK);
#endif

#ifdef SIGQUIT
		signals.add(SIGQUIT);
#endif

		signals.async_wait([&](const boost::system::error_code& error, int /*signal_number*/)
		{
			if (error == boost::asio::error::operation_aborted)
				return;

			server.stop();
		});

		kumori::io_processor processor(service, 2);
		processor.join();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
