#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <kumori/kumori.hpp>

class application_server : public kumori::http_server<application_server>
{
public:

	application_server(boost::asio::io_service& service)
		: service_(service)
		, http_server(service)
		, content_(std::string(1024, 'A'), "text/plain")
	{
	}

	void on_get(kumori::http_server_context& context, bool head)
	{
		content_.on_get(context, head);

		//context.write_headers();

		//auto& stream = context.response_stream();
		//stream << "Hello, World!" << std::endl;

		//kumori::http_client client(service_, "www.yahoo.co.jp", "80");
		//client.request().set_method(kumori::http_method::get);
		//client.request().set_path("/");
		//client.write_headers();
		//client.finish_write();
		//client.read_headers();
		//kumori::forward(client.response_stream(), stream);
		//client.finish_read();
	}

private:

	boost::asio::io_service& service_;

	kumori::static_content content_;

};

int main(int /*argc*/, char* /*argv*/[])
{
	try
	{
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

		signals.async_wait([&](const boost::system::error_code& error, int signal_number)
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
