#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "test_util.hpp"
#include "test_suite.hpp"

#include "../cpp.03/allocation/server.hpp"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <signal.h>

static boost::asio::io_context* ptr_io_context;
volatile _crt_signal_t sig_handler = nullptr;

void sighandler(int sig)
{
	ptr_io_context->stop();
	if (!sig_handler)
		signal(sig, sig_handler);	// rollback
	else
		signal(sig, SIG_IGN);
}

	//#include "../cpp.03/allocation/server.hpp"

TEST(CPLUSPLUS_03, AllocatorHandler) {

	boost::asio::io_context io_context;

	GIVEN("Server �ߴ��� �ʿ��� ���, Ctrl + c");
	ptr_io_context = &io_context;
	sig_handler = signal(SIGINT, sighandler);

	FEATURE("boost::asio::bind_allocator�� ���� ���� ���� �˾ƾ� �Ѵ�");
	using namespace std;
	server s(io_context, 5597);

	boost::thread t(boost::bind(&boost::asio::io_context::run, &io_context));
	t.join();
}

