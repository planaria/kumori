#pragma once
#include "archive_mode.hpp"
#include "serialization_traits.hpp"
#include "string_code.hpp"
#include "archive_exception.hpp"

namespace kumori
{

	template <bool Versioning>
	class input_archive : boost::noncopyable
	{
	public:

		static const archive_mode mode = archive_mode::input;

		input_archive()
		{
		}

		explicit input_archive(std::string str)
			: str_(std::move(str))
		{
		}

		template <class Value>
		input_archive& operator ()(Value& value)
		{
			serialize_impl(value);
			return *this;
		}

		template <class Value, class... Values>
		input_archive& operator ()(Value& value, Values&... values)
		{
			(*this)(value);
			(*this)(values...);
			return *this;
		}

	private:

		template <class Value>
		void serialize_impl(Value& value, typename std::enable_if<serialization_traits<typename std::remove_cv<Value>::type>::primitive>::type* = 0)
		{
			load(value);
		}

		template <class Value>
		void serialize_impl(Value& value, typename std::enable_if<!serialization_traits<typename std::remove_cv<Value>::type>::primitive>::type* = 0)
		{
			std::uint32_t version = 0;
			if (Versioning)
				load(version);
			serialize(*this, value, version);
		}

		std::uint8_t load_byte()
		{
			std::uint8_t value;
			load(value);
			return value;
		}

		void load(char& value)
		{
			if (index_ == str_.size())
				BOOST_THROW_EXCEPTION(archive_exception());

			value = str_[index_++];
		}

		void load(std::uint8_t& value)
		{
			load(reinterpret_cast<char&>(value));
		}

		void load(std::uint16_t& value)
		{
			value = static_cast<std::uint16_t>(load_byte()) << 8;
			value |= static_cast<std::uint16_t>(load_byte());
		}

		void load(std::uint32_t& value)
		{
			value = static_cast<std::uint32_t>(load_byte()) << 24;
			value |= static_cast<std::uint32_t>(load_byte()) << 16;
			value |= static_cast<std::uint32_t>(load_byte()) << 8;
			value |= static_cast<std::uint32_t>(load_byte());
		}

		void load(std::uint64_t& value)
		{
			value = static_cast<std::uint64_t>(load_byte()) << 56;
			value |= static_cast<std::uint64_t>(load_byte()) << 48;
			value |= static_cast<std::uint64_t>(load_byte()) << 40;
			value |= static_cast<std::uint64_t>(load_byte()) << 32;
			value |= static_cast<std::uint64_t>(load_byte()) << 24;
			value |= static_cast<std::uint64_t>(load_byte()) << 16;
			value |= static_cast<std::uint64_t>(load_byte()) << 8;
			value |= static_cast<std::uint64_t>(load_byte());
		}

		void load(std::int8_t& value)
		{
			load(reinterpret_cast<std::uint8_t&>(value));
			value ^= 0x80;
		}

		void load(std::int16_t& value)
		{
			load(reinterpret_cast<std::uint16_t&>(value));
			value ^= 0x8000;
		}

		void load(std::int32_t& value)
		{
			load(reinterpret_cast<std::uint32_t&>(value));
			value ^= 0x80000000;
		}

		void load(std::int64_t& value)
		{
			load(reinterpret_cast<std::uint64_t&>(value));
			value ^= 0x8000000000000000;
		}

		void load(bool& value)
		{
			std::uint8_t v = load_byte();

			if(v != 0 && v != 1)
				BOOST_THROW_EXCEPTION(archive_exception());

			value = !!v;
		}

		void load(float& value)
		{
			std::uint32_t& v = reinterpret_cast<std::uint32_t&>(value);

			load(v);

			if (v & 0x80000000)
				v ^= 0x80000000;
			else
				v = ~v;
		}

		void load(double& value)
		{
			std::uint64_t& v = reinterpret_cast<std::uint64_t&>(value);

			load(v);

			if (v & 0x8000000000000000)
				v ^= 0x8000000000000000;
			else
				v = ~v;
		}

		void load(std::string& value)
		{
			value.clear();

			while (true)
			{
				char c;
				load(c);

				if (c == 0)
				{
					std::uint8_t code = load_byte();

					switch (code)
					{
					case string_code_end:
						return;
					case string_code_null:
						value.push_back('\0');
						break;
					default:
						BOOST_THROW_EXCEPTION(archive_exception());
					}
				}
				else
				{
					value.push_back(c);
				}
			}
		}

		void load(const char* value)
		{
			std::string check_str;
			load(check_str);

			if(check_str != value)
				BOOST_THROW_EXCEPTION(archive_exception());
		}

		std::string str_;
		std::size_t index_ = 0;

	};

	typedef input_archive<false> input_key_archive;
	typedef input_archive<true> input_value_archive;

}
