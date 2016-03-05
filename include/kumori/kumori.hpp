#pragma once

#include <cstddef>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <bitset>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <regex>
#include <random>
#include <utility>

#define BOOST_ASIO_HAS_MOVE

#ifdef _MSC_VER
#define BOOST_USE_WINFIBERS
#endif

#include <boost/filesystem.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/context/all.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/utility.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/cast.hpp>

#include <openssl/md5.h>
#include <openssl/hmac.h>

#ifdef _MSC_VER
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif

#include <picojson.h>

#include <cb_tree/cb_tree.hpp>

#include "server.hpp"
#include "http_server.hpp"

#include "client.hpp"
#include "http_client.hpp"
#include "redis_client.hpp"

#include "null_content.hpp"
#include "static_content.hpp"
#include "regex_content.hpp"
#include "moved_content.hpp"
#include "oauth_content.hpp"
#include "oauth_callback_content.hpp"
#include "directory_content.hpp"

#include "redis_database.hpp"

#include "oauth_twitter.hpp"
#include "oauth_google.hpp"

#include "io_processor.hpp"
