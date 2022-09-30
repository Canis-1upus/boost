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

	std::string set_data = "�׽�Ʈ ������ ���� üũ �Դϴ�.";
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

	GIVEN("Server �ߴ��� �ʿ��� ���, Ctrl + c");
	ptr_io_context = &io_context;
	sig_handler = signal(SIGINT, sighandler);

	FEATURE("boost::asio::bind_allocator�� ���� ���� ���� �˾ƾ� �Ѵ�");
	using namespace std;
	allocation::server s(io_context, 5597);

	boost::thread t(boost::bind(&boost::asio::io_context::run, &io_context));
	t.join();
}


TEST(CPLUSPLUS_03, MockClientConnect) {
	FEATURE("Mock Ŭ���̾�Ʈ�� �Ϲ����� ��� �׽�Ʈ");

	SCENARIO("�ӽ� ����ȯ���� Thread�� ����, �ش� ������ �⺻����");
	SCENARIO("Connect, Send ���� ����� �׽�Ʈ");

	GIVEN("Server PORT: 5597, Address�� loopback���� endpoint ����");
	GIVEN("Server IO�� �ߴ��� ���ؼ�, Ctrl + c");

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


