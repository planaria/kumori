#pragma once
#include "client.hpp"
#include "redis_client_config.hpp"
#include "resp.hpp"
#include "redis_client_type.hpp"
#include "redis_exception.hpp"
#include "stream_util.hpp"

namespace kumori
{

	class redis_client : private client
	{
	public:

		redis_client(
			boost::asio::io_service& service,
			const std::string& host,
			unsigned short port,
			const redis_client_config& config = redis_client_config())
			: client(service, host, port, config)
		{
			this->stream().exceptions(std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);
		}

		template <class Command, class... Args>
		resp command(const Command& cmd, const Args&... args)
		{
			std::ostream& stream = this->stream();
			stream << '*' << (sizeof...(args) + 1) << "\r\n";
			write_params(cmd);
			write_params(args...);
			stream << std::flush;

			return read_response();
		}
		
		// Connection

		template <class Password>
		void auth(const Password& password)
		{
			check_error(command("AUTH", password));
		}

		void ping()
		{
			check_error(command("PING"));
		}

		void quit()
		{
			check_error(command("QUIT"));
		}

		template <class Index>
		void select(const Index& index)
		{
			check_error(command("SELECT", index));
		}

		// Keys

		template <class Key, class... Keys>
		std::int64_t del(const Key& key, const Keys&... keys)
		{
			return as_integer(command("DEL", key, keys...));
		}

		template <class Key>
		std::string dump(const Key& key)
		{
			return as_string(command("DUMP", key));
		}

		template <class Key, class... Keys>
		std::int64_t exists(const Key& key, const Keys&... keys)
		{
			return as_integer(command("EXISTS", key, keys...));
		}

		template <class Key>
		bool expire(const Key& key, std::int64_t seconds)
		{
			return as_integer(command("EXPIRE", key, seconds)) != 0;
		}

		template <class Key>
		bool expireat(const Key& key, std::int64_t timestamp)
		{
			return as_integer(command("EXPIREAT", key, timestamp)) != 0;
		}

		template <class Pattern>
		std::vector<resp> keys(const Pattern& pattern)
		{
			return as_array(command("KEYS", pattern));
		}

		template <class Key>
		bool move(const Key& key, std::int64_t database)
		{
			return as_integer(command("MOVE", key, database)) != 0;
		}

		template <class Key>
		std::int64_t object_refcount(const Key& key)
		{
			return as_integer(command("OBJECT", "REFCOUNT", key));
		}

		template <class Key>
		std::string object_encoding(const Key& key)
		{
			return as_string(command("OBJECT", "ENCODING", key));
		}

		template <class Key>
		std::int64_t object_idletime(const Key& key)
		{
			return as_integer(command("OBJECT", "IDLETIME", key));
		}

		template <class Key>
		bool persist(const Key& key)
		{
			return as_integer(command("PERSIST", key)) != 0;
		}

		template <class Key>
		bool pexpire(const Key& key, std::int64_t milliseconds)
		{
			return as_integer(command("PEXPIRE", key, milliseconds)) != 0;
		}

		template <class Key>
		bool pexpireat(const Key& key, std::int64_t milliseconds_time_stamp)
		{
			return as_integer(command("PEXPIREAT", key, milliseconds_time_stamp)) != 0;
		}

		template <class Key>
		std::int64_t pttl(const Key& key)
		{
			return as_integer(command("PTTL", key));
		}

		boost::optional<std::string> randomkey()
		{
			return as_string_optional(command("RANDOMKEY"));
		}

		template <class Key, class NewKey>
		void rename(const Key& key, const NewKey& new_key)
		{
			check_error(command("RENAME", key, new_key));
		}

		template <class Key, class NewKey>
		bool renamenx(const Key& key, const NewKey& new_key)
		{
			return as_integer(command("RENAMENX", key, new_key)) != 0;
		}

		template <class Key, class Ttl, class SerializedValue>
		void restore(const Key& key, const Ttl& ttl, const SerializedValue& serialized_value, bool replace)
		{
			if (replace)
				check_error(command("RESTORE", key, ttl, serialized_value, "REPLACE"));
			else
				check_error(command("RESTORE", key, ttl, serialized_value));
		}

		template <class Key, class... Args>
		std::vector<resp> sort(const Key& key, const Args&... args)
		{
			return as_array(command("SORT", key, args...));
		}

		template <class Key>
		std::int64_t ttl(const Key& key)
		{
			return as_integer(command("TTL", key));
		}

		template <class Key>
		std::string type(const Key& key)
		{
			return as_string(command("TYPE", key));
		}

		std::int64_t wait(std::int64_t num_slaves, std::int64_t timeout)
		{
			return as_integer(command("WAIT", num_slaves, timeout));
		}

		template <class... Args>
		std::vector<resp> scan(std::int64_t cursor, const Args&... args)
		{
			return as_array(command("SCAN", cursor, args...));
		}

		// Lists

		template <class Key, class... Args>
		std::vector<resp> blpop(const Key& key, const Args&... args)
		{
			return as_array(command("BLPOP", key, args...));
		}

		template <class Key, class... Args>
		std::vector<resp> brpop(const Key& key, const Args&... args)
		{
			return as_array(command("BRPOP", key, args...));
		}

		template <class Source, class Destination>
		boost::optional<std::string> brpoplpush(const Source& source, const Destination& destination, std::int64_t timeout)
		{
			return as_string_optional(command("BRPOPLPUSH", source, destination, timeout));
		}

		template <class Key>
		boost::optional<std::string> lindex(const Key& key, std::int64_t index)
		{
			return as_string_optional(command("LINDEX", key, index));
		}

		template <class Key, class Pivot, class Value>
		std::int64_t linsert(const Key& key, bool after, const Pivot& pivot, const Value& value)
		{
			return as_integer(command("LINSERT", key, after ? "AFTER" : "BEFORE", pivot, value));
		}

		template <class Key>
		std::int64_t llen(const Key& key)
		{
			return as_integer(command("LLEN", key));
		}

		template <class Key>
		boost::optional<std::string> lpop(const Key& key)
		{
			return as_string_optional(command("LPOP", key));
		}

		template <class Key, class Value, class... Values>
		std::int64_t lpush(const Key& key, const Value& value, const Values&... values)
		{
			return as_integer(command("LPUSH", key, value, values...));
		}

		template <class Key, class Value, class... Values>
		std::int64_t lpushx(const Key& key, const Value& value, const Values&... values)
		{
			return as_integer(command("LPUSHX", key, value, values...));
		}

		template <class Key>
		std::vector<resp> lrange(const Key& key, std::int64_t start, std::int64_t stop)
		{
			return as_array(command("LRANGE", key, start, stop));
		}

		template <class Key, class Value>
		std::int64_t lrem(const Key& key, std::int64_t count, const Value& value)
		{
			return as_integer(command("LREM", key, count, value));
		}

		template <class Key, class Value>
		void lset(const Key& key, std::int64_t index, const Value& value)
		{
			check_error(command("LSET", key, index, value));
		}

		template <class Key>
		void ltrim(const Key& key, std::int64_t start, std::int64_t stop)
		{
			check_error(command("LTRIM", key, start, stop));
		}

		template <class Key>
		boost::optional<std::string> rpop(const Key& key)
		{
			return as_string_optional(command("RPOP", key));
		}

		template <class Source, class Destination>
		boost::optional<std::string> rpoplpush(const Source& source, const Destination& destination)
		{
			return as_string_optional(command("RPOPLPUSH", source, destination));
		}

		template <class Key, class Value, class... Values>
		std::int64_t rpush(const Key& key, const Value& value, const Values&... values)
		{
			return as_integer(command("RPUSH", key, value, values...));
		}

		template <class Key, class Value, class... Values>
		std::int64_t rpushx(const Key& key, const Value& value, const Values&... values)
		{
			return as_integer(command("RPUSHX", key, value, values...));
		}

		// Hashes

		template <class Key, class Field, class... Fields>
		std::int64_t hdel(const Key& key, const Field& field, const Fields&... fields)
		{
			return as_integer(command("HDEL", key, field, fields...));
		}

		template <class Key, class Field>
		std::int64_t hexists(const Key& key, const Field& field)
		{
			return as_integer(command("HEXISTS", key, field));
		}

		template <class Key, class Field>
		boost::optional<std::string> hget(const Key& key, const Field& field)
		{
			return as_string_optional(command("HGET", key, field));
		}

		template <class Key>
		std::vector<resp> hgetall(const Key& key)
		{
			return as_array(command("HGETALL", key));
		}

		template <class Key, class Field>
		std::int64_t hincrby(const Key& key, const Field& field, std::int64_t increment)
		{
			return as_integer(command("HINCRBY", key, field, increment));
		}

		template <class Key, class Field>
		double hincrbyfloat(const Key& key, const Field& field, double increment)
		{
			return boost::lexical_cast<double>(as_string(command("HINCRBYFLOAT", key, field, increment)));
		}

		template <class Key>
		std::vector<resp> hkeys(const Key& key)
		{
			return as_array(command("HKEYS", key));
		}

		template <class Key>
		std::int64_t hlen(const Key& key)
		{
			return as_integer(command("HLEN", key));
		}

		template <class Key, class Field, class... Fields>
		std::vector<resp> hmget(const Key& key, const Field& field, const Fields&... fields)
		{
			return as_array(command("HMGET", key, field, fields...));
		}

		template <class Key, class Field, class Value, class... Args>
		void hmset(const Key& key, const Field& field, const Value& value, const Args&... args)
		{
			static_assert(sizeof...(args) % 2 == 0, "args");
			check_error(command("HMSET", key, field, value, args...));
		}

		template <class Key, class Field, class Value>
		bool hset(const Key& key, const Field& field, const Value& value)
		{
			return as_integer(command("HSET", key, field, value)) != 0;
		}

		template <class Key, class Field, class Value>
		bool hsetnx(const Key& key, const Field& field, const Value& value)
		{
			return as_integer(command("HSETNX", key, field, value)) != 0;
		}

		template <class Key, class Field>
		std::int64_t hstrlen(const Key& key, const Field& field)
		{
			return as_integer(command("HSTRLEN", key, field));
		}

		template <class Key>
		std::vector<resp> hvals(const Key& key)
		{
			return as_array(command("HVALS", key));
		}

		template <class Key, class... Args>
		std::vector<resp> hscan(const Key& key, std::int64_t cursor, const Args&... args)
		{
			return as_array(command("HSCAN", key, cursor, args...));
		}

		// Strings

		template <class Key, class Value>
		std::int64_t append(const Key& key, const Value& value)
		{
			return as_integer(command("APPEND", key, value));
		}

		template <class Key>
		std::int64_t bitcount(const Key& key)
		{
			return as_integer(command("BITCOUNT", key));
		}

		template <class Key>
		std::int64_t bitcount(const Key& key, std::int64_t start, std::int64_t end)
		{
			return as_integer(command("BITCOUNT", key, start, end));
		}

		template <class DestKey, class SrcKey, class... SrcKeys>
		std::int64_t bitop_and(const DestKey& dest_key, const SrcKey& src_key, const SrcKeys&... src_keys)
		{
			return as_integer(command("BITOP", "AND", dest_key, src_key, src_keys...));
		}

		template <class DestKey, class SrcKey, class... SrcKeys>
		std::int64_t bitop_or(const DestKey& dest_key, const SrcKey& src_key, const SrcKeys&... src_keys)
		{
			return as_integer(command("BITOP", "OR", dest_key, src_key, src_keys...));
		}

		template <class DestKey, class SrcKey, class... SrcKeys>
		std::int64_t bitop_xor(const DestKey& dest_key, const SrcKey& src_key, const SrcKeys&... src_keys)
		{
			return as_integer(command("BITOP", "XOR", dest_key, src_key, src_keys...));
		}

		template <class DestKey, class SrcKey>
		std::int64_t bitop_xor(const DestKey& dest_key, const SrcKey& src_key)
		{
			return as_integer(command("BITOP", "NOT", dest_key, src_key));
		}

		template <class Key>
		std::int64_t bitpos(const Key& key, bool bit)
		{
			return as_integer(command("BITPOS", key, bit ? "1" : "0"));
		}

		template <class Key>
		std::int64_t bitpos(const Key& key, bool bit, std::int64_t start)
		{
			return as_integer(command("BITPOS", key, bit ? "1" : "0", start));
		}

		template <class Key>
		std::int64_t bitpos(const Key& key, bool bit, std::int64_t start, std::int64_t end)
		{
			return as_integer(command("BITPOS", key, bit ? "1" : "0", start, end));
		}

		template <class Key>
		std::int64_t decr(const Key& key)
		{
			return as_integer(command("DECR", key));
		}

		template <class Key>
		std::int64_t decrby(const Key& key, std::int64_t decrement)
		{
			return as_integer(command("DECRBY", key, decrement));
		}

		template <class Key>
		boost::optional<std::string> get(const Key& key)
		{
			return as_string_optional(command("GET", key));
		}

		template <class Key>
		bool getbit(const Key& key, std::int64_t offset)
		{
			return as_integer(command("GETBIT", key, offset)) != 0;
		}

		template <class Key>
		std::string getrange(const Key& key, std::int64_t start, std::int64_t end)
		{
			return as_string(command("GETRANGE", key, start, end));
		}

		template <class Key, class Value>
		boost::optional<std::string> getset(const Key& key, const Value& value)
		{
			return as_string_optional(command("GETSET", key, value));
		}

		template <class Key>
		std::int64_t incr(const Key& key)
		{
			return as_integer(command("INCR", key));
		}

		template <class Key>
		std::int64_t incrby(const Key& key, std::int64_t increment)
		{
			return as_integer(command("INCRBY", key, increment));
		}

		template <class Key>
		double incrbyfloat(const Key& key, double increment)
		{
			return boost::lexical_cast<double>(as_string(command("INCRBYFLOAT", key, increment)));
		}

		template <class Key, class... Keys>
		std::vector<resp> mget(const Key& key, const Keys&... keys)
		{
			return as_array(command("MGET", key, keys...));
		}

		template <class Key, class Value, class... Args>
		void mset(const Key& key, const Value& value, const Args&... args)
		{
			static_assert(sizeof...(args) % 2 == 0, "args");
			check_error(command("MSET", key, value, args...));
		}

		template <class Key, class Value, class... Args>
		bool msetnx(const Key& key, const Value& value, const Args&... args)
		{
			static_assert(sizeof...(args) % 2 == 0, "args");
			return as_integer(command("MSETNX", key, value, args...)) != 0;
		}

		template <class Key, class Value>
		void set(const Key& key, const Value& value)
		{
			check_error(command("SET", key, value));
		}

		template <class Key, class Value>
		void set_ex(const Key& key, const Value& value, std::int64_t seconds)
		{
			check_error(command("SET", key, value, "EX", seconds));
		}

		template <class Key, class Value>
		void set_px(const Key& key, const Value& value, std::int64_t seconds)
		{
			check_error(command("SET", key, value, "PX", seconds));
		}

		template <class Key, class Value>
		void setnx(const Key& key, const Value& value)
		{
			check_error(command("SET", key, value, "NX"));
		}

		template <class Key, class Value>
		void setnx_ex(const Key& key, const Value& value, std::int64_t seconds)
		{
			check_error(command("SET", key, value, "EX", seconds, "NX"));
		}

		template <class Key, class Value>
		void setnx_px(const Key& key, const Value& value, std::int64_t seconds)
		{
			check_error(command("SET", key, value, "PX", seconds, "NX"));
		}

		template <class Key, class Value>
		void setxx(const Key& key, const Value& value)
		{
			check_error(command("SET", key, value, "XX"));
		}

		template <class Key, class Value>
		void setxx_ex(const Key& key, const Value& value, std::int64_t seconds)
		{
			check_error(command("SET", key, value, "EX", seconds, "XX"));
		}

		template <class Key, class Value>
		void setxx_px(const Key& key, const Value& value, std::int64_t seconds)
		{
			check_error(command("SET", key, value, "PX", seconds, "XX"));
		}

		template <class Key, class Value>
		bool setbit(const Key& key, std::int64_t offset, const Value& value)
		{
			return as_integer(command("SETBIT", key, offset, value)) != 0;
		}

		template <class Key, class Value>
		std::int64_t setrange(const Key& key, std::int64_t offset, const Value& value)
		{
			return as_integer(command("SETRANGE", key, offset, value));
		}

		template <class Key>
		std::int64_t strlen(const Key& key)
		{
			return as_integer(command("STRLEN", key));
		}

		// Sets

		template <class Key, class Member, class... Members>
		std::int64_t sadd(const Key& key, const Member& member, const Members&... members)
		{
			return as_integer(command("SADD", key, member, members...));
		}

		template <class Key>
		std::int64_t scard(const Key& key)
		{
			return as_integer(command("SCARD", key));
		}

		template <class Key, class... Keys>
		std::vector<resp> sdiff(const Key& key, const Keys&... keys)
		{
			return as_array(command("SDIFF", key, keys...));
		}

		template <class Destination, class Key, class... Keys>
		std::int64_t sdiffstore(const Destination& destination, const Key& key, const Keys&... keys)
		{
			return as_integer(command("SDIFFSTORE", destination, key, keys...));
		}

		template <class Key, class... Keys>
		std::vector<resp> sinter(const Key& key, const Keys&... keys)
		{
			return as_array(command("SINTER", key, keys...));
		}

		template <class Destination, class Key, class... Keys>
		std::int64_t sinterstore(const Destination& destination, const Key& key, const Keys&... keys)
		{
			return as_integer(command("SINTERSTORE", destination, key, keys...));
		}

		template <class Key, class Member>
		bool sismember(const Key& key, const Member& member)
		{
			return as_integer(command("SISMEMBER", key, member)) != 0;
		}

		template <class Key>
		std::vector<resp> smembers(const Key& key)
		{
			return as_array(command("SMEMBERS", key));
		}

		template <class Source, class Destination, class Member>
		bool smove(const Source& source, const Destination& destination, const Member& member)
		{
			return as_integer(command("SMOVE", source, destination, member)) != 0;
		}

		template <class Key>
		boost::optional<std::string> spop(const Key& key)
		{
			return as_string_optional(command("SPOP", key));
		}

		template <class Key>
		std::vector<resp> spop(const Key& key, std::int64_t count)
		{
			return as_array(command("SPOP", key, count));
		}

		template <class Key>
		boost::optional<std::string> srandmember(const Key& key)
		{
			return as_string_optional(command("SRANDMEMBER", key));
		}

		template <class Key>
		std::vector<resp> srandmember(const Key& key, std::int64_t count)
		{
			return as_array(command("SRANDMEMBER", key, count));
		}

		template <class Key, class Member, class... Members>
		std::int64_t srem(const Key& key, const Member& member, const Members&... members)
		{
			return as_integer(command("SREM", key, member, members...));
		}

		template <class Key, class... Keys>
		std::vector<resp> sunion(const Key& key, const Keys&... keys)
		{
			return as_array(command("SUNION", key, keys...));
		}

		template <class Destination, class Key, class... Keys>
		std::int64_t sunionstore(const Destination& destination, const Key& key, const Keys&... keys)
		{
			return as_integer(command("SUNIONSTORE", destination, key, keys...));
		}

		template <class Key, class... Args>
		std::vector<resp> sscan(const Key& key, std::int64_t cursor, const Args&... args)
		{
			return as_array(command("SSCAN", key, cursor, args...));
		}

		// Sorted Sets

		template <class Key, class Score, class Member, class... Args>
		std::int64_t zadd(const Key& key, const Score& score, const Member& member, const Args&... args)
		{
			static_assert(sizeof...(args) % 2 == 0, "args");
			return as_integer(command("ZADD", key, score, member, args...));
		}

		template <class Key, class Score, class Member, class... Args>
		std::int64_t zaddnx(const Key& key, const Score& score, const Member& member, const Args&... args)
		{
			static_assert(sizeof...(args) % 2 == 0, "args");
			return as_integer(command("ZADD", key, "NX", score, member, args...));
		}

		template <class Key, class Score, class Member, class... Args>
		std::int64_t zaddxx(const Key& key, const Score& score, const Member& member, const Args&... args)
		{
			static_assert(sizeof...(args) % 2 == 0, "args");
			return as_integer(command("ZADD", key, "XX", score, member, args...));
		}

		template <class Key>
		std::int64_t zcard(const Key& key)
		{
			return as_integer(command("ZCARD", key));
		}

		template <class Key, class Min, class Max>
		std::int64_t zcount(const Key& key, const Min& min, const Max& max)
		{
			return as_integer(command("ZCOUNT", key, min, max));
		}

		template <class Key, class Member>
		double zincrby(const Key& key, double increment, const Member& member)
		{
			return boost::lexical_cast<double>(as_string(command("ZINCRBY", key, increment, member)));
		}

		template <class Destination, class Key, class... Args>
		std::int64_t zinterstore(const Destination& destination, std::int64_t num_keys, const Key& key, const Args&... args)
		{
			return as_integer(command("ZINTERSTORE", destination, num_keys, key, args...));
		}

		template <class Key, class Min, class Max>
		std::int64_t zlexcount(const Key& key, const Min& min, const Max& max)
		{
			return as_integer(command("ZLEXCOUNT", key, min, max));
		}

		template <class Key>
		std::vector<resp> zrange(const Key& key, std::int64_t start, std::int64_t stop)
		{
			return as_array(command("ZRANGE", key, start, stop));
		}

		template <class Key>
		std::vector<resp> zrange_withscores(const Key& key, std::int64_t start, std::int64_t stop)
		{
			return as_array(command("ZRANGE", key, start, stop, "WITHSCORES"));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrangebylex(const Key& key, const Min& min, const Max& max)
		{
			return as_array(command("ZRANGEBYLEX", key, min, max));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrangebylex_limit(const Key& key, const Min& min, const Max& max, std::int64_t offset, std::int64_t count)
		{
			return as_array(command("ZRANGEBYLEX", key, min, max, "LIMIT", offset, count));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrevrangebylex(const Key& key, const Min& min, const Max& max)
		{
			return as_array(command("ZREVRANGEBYLEX", key, min, max));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrevrangebylex_limit(const Key& key, const Min& min, const Max& max, std::int64_t offset, std::int64_t count)
		{
			return as_array(command("ZREVRANGEBYLEX", key, min, max, "LIMIT", offset, count));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrangebyscore(const Key& key, const Min& min, const Max& max)
		{
			return as_array(command("ZRANGEBYSCORE", key, min, max));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrangebyscore_withscores(const Key& key, const Min& min, const Max& max)
		{
			return as_array(command("ZRANGEBYSCORE", key, min, max, "WITHSCORES"));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrangebyscore_limit(const Key& key, const Min& min, const Max& max, std::int64_t offset, std::int64_t count)
		{
			return as_array(command("ZRANGEBYSCORE", key, min, max, "LIMIT", offset, count));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrangebyscore_withscores_limit(const Key& key, const Min& min, const Max& max, std::int64_t offset, std::int64_t count)
		{
			return as_array(command("ZRANGEBYSCORE", key, min, max, "WITHSCORES", "LIMIT", offset, count));
		}

		template <class Key, class Member>
		boost::optional<std::int64_t> zrank(const Key& key, const Member& member)
		{
			return as_integer_optional(command("ZRANK", key, member));
		}

		template <class Key, class Member, class... Members>
		std::int64_t zrem(const Key& key, const Member& member, const Members&... members)
		{
			return as_integer(command("ZREM", key, member, members...));
		}

		template <class Key, class Min, class Max>
		std::int64_t zremrangebylex(const Key& key, const Min& min, const Max& max)
		{
			return as_integer(command("ZREMRANGEBYLEX", key, min, max));
		}

		template <class Key>
		std::int64_t zremrangebyrank(const Key& key, std::int64_t start, std::int64_t stop)
		{
			return as_integer(command("ZREMRANGEBYRANK", key, start, stop));
		}

		template <class Key, class Min, class Max>
		std::int64_t zremrangebyscore(const Key& key, const Min& min, const Max& max)
		{
			return as_integer(command("ZREMRANGEBYSCORE", key, min, max));
		}

		template <class Key>
		std::vector<resp> zrevrange(const Key& key, std::int64_t start, std::int64_t stop)
		{
			return as_array(command("ZREVRANGE", key, start, stop));
		}

		template <class Key>
		std::vector<resp> zrevrange_withscores(const Key& key, std::int64_t start, std::int64_t stop)
		{
			return as_array(command("ZREVRANGE", key, start, stop, "WITHSCORES"));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrevrangebyscore(const Key& key, const Min& min, const Max& max)
		{
			return as_array(command("ZREVRANGEBYSCORE", key, min, max));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrevrangebyscore_withscores(const Key& key, const Min& min, const Max& max)
		{
			return as_array(command("ZREVRANGEBYSCORE", key, min, max, "WITHSCORES"));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrevrangebyscore_limit(const Key& key, const Min& min, const Max& max, std::int64_t offset, std::int64_t count)
		{
			return as_array(command("ZREVRANGEBYSCORE", key, min, max, "LIMIT", offset, count));
		}

		template <class Key, class Min, class Max>
		std::vector<resp> zrevrangebyscore_withscores_limit(const Key& key, const Min& min, const Max& max, std::int64_t offset, std::int64_t count)
		{
			return as_array(command("ZREVRANGEBYSCORE", key, min, max, "WITHSCORES", "LIMIT", offset, count));
		}

		template <class Key, class Member>
		boost::optional<std::int64_t> zrevrank(const Key& key, const Member& member)
		{
			return as_integer_optional(command("ZREVRANK", key, member));
		}

		template <class Key, class Member>
		double zscore(const Key& key, const Member& member)
		{
			return boost::lexical_cast<double>(as_string(command("ZSCORE", key, member)));
		}

		template <class Destination, class Key, class... Args>
		std::int64_t zunionstore(const Destination& destination, std::int64_t num_keys, const Key& key, const Args&... args)
		{
			return as_integer(command("ZUNIONSTORE", destination, num_keys, key, args...));
		}

		template <class Key, class... Args>
		std::vector<resp> zscan(const Key& key, std::int64_t cursor, const Args&... args)
		{
			return as_array(command("ZSCAN", key, cursor, args...));
		}

		// Transactions

		void discard()
		{
			check_error(command("DISCARD"));
		}

		std::vector<resp> exec()
		{
			return as_array(command("EXEC"));
		}

		void multi()
		{
			check_error(command("MULTI"));
		}

		void unwatch()
		{
			check_error(command("UNWATCH"));
		}

		template <class Key, class... Keys>
		void watch(const Key& key, const Keys&... keys)
		{
			check_error(command("WATCH", key, keys...));
		}

	private:

		void write_params()
		{
		}

		template <class... Args>
		void write_params(const char* param, const Args&... args)
		{
			this->stream() << '$' << std::strlen(param) << "\r\n" << param << "\r\n";
			write_params(args...);
		}

		template <class... Args>
		void write_params(const std::string& param, const Args&... args)
		{
			this->stream() << '$' << param.size() << "\r\n" << param << "\r\n";
			write_params(args...);
		}

		template <class... Args>
		void write_params(std::int64_t param, const Args&... args)
		{
			write_params(boost::lexical_cast<std::string>(param), args...);
		}

		template <class... Args>
		void write_params(int param, const Args&... args)
		{
			write_params(boost::lexical_cast<std::string>(param), args...);
		}

		template <class... Args>
		void write_params(double param, const Args&... args)
		{
			write_params(boost::lexical_cast<std::string>(param), args...);
		}

		template <class... Args>
		void write_params(redis_client_type param, const Args&... args)
		{
			write_params(boost::lexical_cast<std::string>(param), args...);
		}

		resp read_response()
		{
			std::istream& stream = this->stream();

			int type = stream.get();

			switch (type)
			{
			case '+':
				return resp_string(read_until_crlf());
			case '-':
				return resp_error(read_until_crlf());
			case ':':
				return resp_integer(boost::lexical_cast<std::int64_t>(read_until_crlf()));
			case '$':
			{
				std::int64_t size64 = boost::lexical_cast<std::int64_t>(read_until_crlf());
				if (size64 == -1)
					return resp_nil();

				std::size_t size = boost::numeric_cast<std::size_t>(size64);
				std::string value;
				value.reserve(size);
				
				for (std::size_t i = 0; i < size; ++i)
					value.push_back(static_cast<char>(stream.get()));

				read_crlf();

				return resp_string(std::move(value));
			}
			case '*':
			{
				std::int64_t size64 = boost::lexical_cast<std::int64_t>(read_until_crlf());
				if (size64 == -1)
					return resp_nil();

				std::size_t size = boost::numeric_cast<std::size_t>(size64);
				std::vector<resp> values(size);

				for (std::size_t i = 0; i < size; ++i)
					values[i] = read_response();

				return resp_array(std::move(values));
			}
			default:
				BOOST_THROW_EXCEPTION(redis_exception("unknown data type"));
			}
		}

		std::string read_until_crlf()
		{
			std::string result;
			std::istream& stream = this->stream();

			int c;
			while ((c = stream.get()) != '\r')
				result.push_back(static_cast<char>(c));

			if(stream.get() != '\n')
				BOOST_THROW_EXCEPTION(redis_exception("invalid protocol"));

			return result;
		}

		void read_crlf()
		{
			std::istream& stream = this->stream();

			if (stream.get() != '\r')
				BOOST_THROW_EXCEPTION(redis_exception("invalid protocol"));

			if (stream.get() != '\n')
				BOOST_THROW_EXCEPTION(redis_exception("invalid protocol"));
		}

	};

}
