#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "test_util.hpp"
#include "test_suite.hpp"

#include <signal.h>
#include <boost/asio.hpp>

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

#pragma region buffers

#include "../cpp.03/buffers/reference_counted.hpp"

#pragma endregion

#pragma region allocation

#include "../cpp.03/allocation/server.hpp"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>


	//#include "../cpp.03/allocation/server.hpp"

#include "mock_client.hpp"

TEST(CPLUSPLUS_03, MockClientSetSendData)
{
	boost::asio::io_context io_context;
	mock_tcp_client client(io_context);

	std::string set_data = "테스트 데이터 세팅 체크 입니다.";
	client.set_send_data(set_data);

	boost::array<unsigned char, 1024> send_buf = client.get_send_data();

	std::string get_data;
	char temp_buf[1024];
	memset(temp_buf, 0, 1024);
	std::copy(send_buf.begin(), send_buf.end(), temp_buf);
	get_data = temp_buf;

	EXPECT_STREQ(set_data.c_str(), get_data.c_str());
}

TEST(CPLUSPLUS_03, AllocatorHandler) {

	boost::asio::io_context io_context;

	GIVEN("Server 중단이 필요한 경우, Ctrl + c");
	ptr_io_context = &io_context;
	sig_handler = signal(SIGINT, sighandler);

	FEATURE("boost::asio::bind_allocator에 대한 것을 먼저 알아야 한다");
	using namespace std;
	allocation::server s(io_context, 5597);

	boost::thread t(boost::bind(&boost::asio::io_context::run, &io_context));
	t.join();
}


TEST(CPLUSPLUS_03, MockClientConnect) {
	FEATURE("Mock 클라이언트의 일반적인 기능 테스트");

	SCENARIO("임시 서버환경을 Thread로 구동, 해당 서버에 기본적인");
	SCENARIO("Connect, Send 등의 기능을 테스트");

	GIVEN("Server PORT: 5597, Address는 loopback으로 endpoint 지정");
	GIVEN("Server IO의 중단을 위해선, Ctrl + c");

	boost::asio::io_context io_context;
	ptr_io_context = &io_context;
	sig_handler = ::signal(SIGINT, sighandler);
	allocation::server s(io_context, 5597);

	boost::thread th(boost::bind(&boost::asio::io_context::run, &io_context));

	boost::asio::io_context io_client;
	mock_tcp_client client(io_client);

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 5597);

	client.connect(ep);
	io_client.run();
	th.join();
}

#pragma endregion	


