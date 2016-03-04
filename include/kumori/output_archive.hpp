#pragma once
#include "archive_mode.hpp"
#include "serialization_traits.hpp"
#include "string_code.hpp"

namespace kumori
{

	template <bool Versioning>
	class output_archive : boost::noncopyable
	{
	public:

		static const archive_mode mode = archive_mode::output;

		template <class Value>
		output_archive& operator ()(const Value& value)
		{
			serialize_impl(value);
			return *this;
		}

		template <class Value, class... Values>
		output_archive& operator ()(const Value& value, const Values&... values)
		{
			(*this)(value);
			(*this)(values...);
			return *this;
		}

		const std::string& str() const
		{
			return str_;
		}

		std::string detach_str()
		{
			std::string result;
			result.swap(str_);
			return result;
		}

	private:

		template <class Value>
		void serialize_impl(const Value& value, typename std::enable_if<serialization_traits<typename std::remove_cv<Value>::type>::primitive>::type* = 0)
		{
			save(value);
		}

		template <class Value>
		void serialize_impl(const Value& value, typename std::enable_if<!serialization_traits<typename std::remove_cv<Value>::type>::primitive>::type* = 0)
		{
			std::uint32_t version = serialization_traits<typename std::remove_cv<Value>::type>::version;
			if (Versioning)
				save(version);
			serialize(*this, const_cast<Value&>(value), version);
		}

		void save(char value)
		{
			str_.push_back(value);
		}

		void save(std::uint8_t value)
		{
			save(static_cast<char>(value));
		}

		void save(std::uint16_t value)
		{
			save(static_cast<std::uint8_t>(value >> 8));
			save(static_cast<std::uint8_t>(value & 0xff));
		}

		void save(std::uint32_t value)
		{
			save(static_cast<std::uint8_t>(value >> 24));
			save(static_cast<std::uint8_t>((value >> 16) & 0xff));
			save(static_cast<std::uint8_t>((value >> 8) & 0xff));
			save(static_cast<std::uint8_t>(value & 0xff));
		}

		void save(std::uint64_t value)
		{
			save(static_cast<std::uint8_t>(value >> 56));
			save(static_cast<std::uint8_t>((value >> 48) & 0xff));
			save(static_cast<std::uint8_t>((value >> 40) & 0xff));
			save(static_cast<std::uint8_t>((value >> 32) & 0xff));
			save(static_cast<std::uint8_t>((value >> 24) & 0xff));
			save(static_cast<std::uint8_t>((value >> 16) & 0xff));
			save(static_cast<std::uint8_t>((value >> 8) & 0xff));
			save(static_cast<std::uint8_t>(value & 0xff));
		}

		void save(std::int8_t value)
		{
			value ^= 0x80;
			save(static_cast<std::uint8_t>(value));
		}

		void save(std::int16_t value)
		{
			value ^= 0x8000;
			save(static_cast<std::uint16_t>(value));
		}

		void save(std::int32_t value)
		{
			value ^= 0x80000000;
			save(static_cast<std::uint32_t>(value));
		}

		void save(std::int64_t value)
		{
			value ^= 0x8000000000000000;
			save(static_cast<std::uint64_t>(value));
		}

		void save(bool value)
		{
			save(static_cast<std::uint8_t>(value ? 1 : 0));
		}

		void save(float value)
		{
			std::uint32_t v = reinterpret_cast<const std::uint32_t&>(value);

			if (v & 0x80000000)
				v = ~v;
			else
				v ^= 0x80000000;

			save(v);
		}

		void save(double value)
		{
			std::uint64_t v = reinterpret_cast<const std::uint64_t&>(value);

			if (v & 0x8000000000000000)
				v = ~v;
			else
				v ^= 0x8000000000000000;

			save(v);
		}

		void save(const std::string& value)
		{
			save_binary(value.data(), value.size());
		}

		void save(const char* value)
		{
			save_binary(value, std::strlen(value));
		}

		void save_binary(const char* data, std::size_t size)
		{
			for (std::size_t i = 0; i < size; ++i)
			{
				char c = data[i];
				save(c);

				if (c == '\0')
					save(string_code_null);
			}

			save('\0');
			save(string_code_end);
		}

	private:

		std::string str_;

	};

	typedef output_archive<false> output_key_archive;
	typedef output_archive<true> output_value_archive;

}
