#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "test_suite.hpp"

#include "../cpp.03/allocation/server.hpp"
#include <boost/asio.hpp>

TEST(CPLUSPLUS_03, AllocatorHandler) {
	boost::asio::io_context io_context;

	using namespace std;
	server s(io_context, 5597);

	io_context.run();
}