#pragma once
#include "socket.hpp"
#include "sync.hpp"
#include "interrupted_exception.hpp"

namespace kumori
{

	template <class Derived>
	class socket_adaptor : public socket
	{
	public:

		virtual std::size_t read_some(
			char* buffer,
			std::size_t buffer_size,
			const boost::posix_time::time_duration& timeout) override
		{
			sync& sync = sync::current();

			boost::system::error_code error;
			std::size_t length;

			derived().async_read_some(boost::asio::buffer(buffer, buffer_size),
				[&](const boost::system::error_code& e, std::size_t l)
			{
				error = e;
				length = l;
				sync.resume();
			});

			sync.suspend_for(timeout,
				[&]()
			{
				raw_socket().cancel();
			});

			static boost::system::error_code short_read(
				ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SHORT_READ),
				boost::asio::error::get_ssl_category());

			if (!error || error == short_read)
				return length;
			else if (error == boost::asio::error::operation_aborted)
				BOOST_THROW_EXCEPTION(interrupted_exception());
			else if (error == boost::asio::error::eof)
				return 0;
			else
				BOOST_THROW_EXCEPTION(boost::system::system_error(error));
		}

		virtual std::size_t write_some(
			const char* buffer,
			std::size_t buffer_size,
			const boost::posix_time::time_duration& timeout) override
		{
			sync& sync = sync::current();

			boost::system::error_code error;
			std::size_t length;

			derived().async_write_some(boost::asio::buffer(buffer, buffer_size),
				[&](const boost::system::error_code& e, std::size_t l)
			{
				error = e;
				length = l;
				sync.resume();
			});

			sync.suspend_for(timeout,
				[&]()
			{
				raw_socket().cancel();
			});

			if (!error)
				return length;
			else if (error == boost::asio::error::operation_aborted)
				BOOST_THROW_EXCEPTION(interrupted_exception());
			else
				BOOST_THROW_EXCEPTION(boost::system::system_error(error));
		}

	private:

		Derived& derived()
		{
			return static_cast<Derived&>(*this);
		}

	};

}
