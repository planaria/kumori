#pragma once

#include <cstddef>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <regex>
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
#include <boost/utility.hpp>
#include <boost/utility/in_place_factory.hpp>

#include <openssl/md5.h>
#include <openssl/hmac.h>

#ifdef _MSC_VER
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif

#include <cb_tree/cb_tree.hpp>

#include "server.hpp"
#include "http_server.hpp"

#include "client.hpp"
#include "http_client.hpp"

#include "null_content.hpp"
#include "static_content.hpp"
#include "directory_content.hpp"

#include "io_processor.hpp"
