#pragma once

#include <unordered_map>
#include <bitset>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <utility>

#define BOOST_ASIO_HAS_MOVE
#define BOOST_USE_WINFIBERS

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
#include <boost/utility.hpp>

#include "server.hpp"
#include "http_server.hpp"

#include "client.hpp"
#include "http_client.hpp"

#include "io_processor.hpp"
