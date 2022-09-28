#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestSuite.hpp"
#include "TestUtil.hpp"

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>


TEST(TUTORIAL_ASIO, SyncTimer) 
{
	using namespace std::chrono;
	using namespace boost::asio;

	io_context io;

	FEATURE("asio�� io���ۿ� ���� proactor ������ Ȯ���Ѵ�");
	SCENARIO("asio ���񽺿� ���ǵ� ����ȭ Ÿ�̸Ӹ� �۵� �ش� �Լ��� ��ȯ���� �ɸ� �ð��� ���õ� �ð��� �������� ���Ѵ�");

	GIVEN("Ÿ�̸� ���� �ð��� 5(s) �̴�");
	uint32_t sec = 5;
	system_timer timer(io, seconds(sec));

	WHEN("Ÿ�̸� ���� �ð��� üũ�ϰ�, Ÿ�̸Ӹ� ����. �Ϸ� �ð��� üũ");
	system_clock::time_point start = system_clock::now();
	timer.wait();
	system_clock::time_point end = system_clock::now();

	THEN("Ÿ�̸� ������� �ɸ� �ð��� ���� �ð��� ���ų� ũ�� üũ�� ���̴�");

	// �ݿø� ��Ȯ�� ����Ȯ
	////////EXPECT_LE(sec, duration_cast<seconds>(end - start).count());

	// EXPECT_LE(exp, act) := (exp <= act)
	long long actual = std::chrono::round<seconds>(end - start).count();

	RESULT(boost::lexical_cast<std::string>(actual));
	EXPECT_LE(sec, actual);
}

void EndPrint(const boost::system::error_code& e)
{
	std::cout << ">>>>>>>>>>  End, Timer!" << std::endl;
}

class MockAsyncTimer {
public:
	MOCK_METHOD1(endPrint, void(const boost::system::error_code&));

	MockAsyncTimer()
	{
		// delegate
		ON_CALL(*this, endPrint(::testing::_)).WillByDefault(::testing::Invoke(EndPrint));
	}
};

#include <functional>

TEST(TUTORIAL_ASIO, AsyncTimer)
{
	using namespace std::chrono;
	using namespace boost::asio;

	io_context io;

	FEATURE("asio�� io���ۿ� ���� proactor ������ Ȯ���Ѵ�");

	SCENARIO("asio ���񽺿� ���ǵ� �񵿱�ȭ Ÿ�̸Ӹ� �۵�"); 
	SCENARIO("�ش� �Լ��� ��ȯ���� �ɸ� �ð��� ���õ� �ð����� ���� ���̸�,");
	SCENARIO("asio�� io_context::run()�� ������ ������� io �Ϸ� ���� ��ϵ� �Լ��� ������ ���̱⿡");
	SCENARIO("Mock Test�� ���ۿ� ���� �׽�Ʈ Ȯ���� �����ϴ�");

	GIVEN("Ÿ�̸� ���� �ð��� 5(s) �̴�");
	uint32_t sec = 5;
	system_timer timer(io, seconds(sec));

	WHEN("Ÿ�̸� ���� �ð��� üũ�ϰ�, Ÿ�̸Ӹ� ����. �Ϸ� �ð��� üũ");
	system_clock::time_point start = system_clock::now();

	//AND("Mock �׽�Ʈ�� io �Ϸ� ���� �ش� �Լ��� ȣ������� ��� Ȯ��");
	//MockAsyncTimer mock;
	//EXPECT_CALL(mock, endPrint).Times(1);
	//std::function<void(const boost::system::error_code&)> memberFunc
	//  = std::bind(&MockAsyncTimer::endPrint, std::placeholders::_1);
	//timer.async_wait(&memberFunc(mock));

	timer.async_wait(&EndPrint);

	system_clock::time_point end = system_clock::now();

	THEN("Ÿ�̸� ������� �ɸ� �ð��� ���� �ð��� ���࿡ üũ�� �ð����� Ŭ ���̴�.(�񵿱�� �ٷ� ��ȯ�� �߻��ϱ⿡)");
	// EXPECT_LE(exp, act) := (exp <= act)

	RESULT(boost::lexical_cast<std::string>(std::chrono::round<seconds>(end - start).count()));
	EXPECT_GE(sec, duration_cast<seconds>(end - start).count());
}

TEST(TUTORIAL_ASIO, BindArgumentToCompletionHandler)
{
	FEATURE("asio, io �ڵ鷯 �Լ��� �߰����� �Ķ���͸� ���ε� �� �� ������ �����ش�");

	SCENARIO("print �Լ� �� ���� Ÿ�̸Ӹ� �����ϸ� �񵿱� IO�� ��� ������� �Ѵ�");
	SCENARIO("5�� �̻��� ���������� �� �̻� �񵿱� IO �۾��� �߻����� �����Ƿ� �� �̻� �۾��� ���� ���·�");
	SCENARIO("io::run�� ��ȯ�� �߻�");

	boost::asio::io_context io;
	int count = 0;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
	t.async_wait(boost::bind(print, boost::asio::placeholders::error, &t, &count));
	io.run();

	std::cout << "Final count is " << count << std::endl;
}

TEST(TUTORIAL_ASIO, SynchronisingCompletionHandlersInMTP)
{
	FEATURE("��Ƽ������ ȯ�濡�� Completion �ڵ鷯���� ����ȭ�� ���� strand�� ����");

	SCENARIO("�⺻������ Completion �ڵ鷯�� ���� io_context::run �� ȣ��� thread�� ���ؼ��� ȣ�� �ǵ��� ����");
	SCENARIO("���� io_context::run �� ���� �����忡���� ȣ�� �ȴٴ� ���� Completeion �ڵ鷯�� ���� ���ü��� Ȯ���� �� ����");
	SCENARIO("�ᱹ io_context::run�� ������ Ǯ���� ������ �Ѵٴ� �̾߱��ε�, �̷� ���, Completion �ڵ鷯�� ��Ƽ �����忡 ���� ���ÿ� ���� �ǹǷ�, Completion �ڵ鷯 ���� thread-unsafe �� ���� �ڿ��鿡 ���� ����ȭ ����� �ʿ��ϴ�");
	SCENARIO("�̸� ���� ������ �����Ǵ� ����� strand Ŭ���� ���ø� �ΰ� ");
	SCENARIO("strand �� executor ����ͷ�, �̰��� ���� dispatch(�İ�)ed �Ǵ� �ڵ鷯�鿡 ���� ����ȭ�� ���� �Ѵ�");
	SCENARIO("���� ���� �ڵ鷯�� �ٸ� �ڵ鷯�� ����Ǳ� ���� �ݵ�� ���� �Ϸ� �� �� �ֵ��� �����ϴ� ��(�̶� 2���� �ڵ鷯 ��� strand ����͸� ���� ��� �� ���̾�� �Ѵ�)");
	SCENARIO("���� strand �� ���� ��ϵ� �ڵ鷯�� ���� ���ü��� ��� ������ �Ǵ� ��������, ������ �ٸ� strand Ȥ�� ���� ��ϵ� �ٸ� Completion �ڵ鷯�鿡 ���� ���ü��� �����Ѵ�");

	SCENARIO("�� ���� ������ �ʱ�ȭ �� ��, �� �ڵ鷯���� bind_executor �Լ��� strand ��ü�� ���ε� �Ǿ� ���� �� �� �ֵ��� �Ѵ�");

	boost::asio::io_context io;
	printer p(io);

	boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
	io.run();
	t.join();
}

#include "daytime.hpp"

TEST(TUTORIAL_ASIO, SyncDaytimeClientServer)
{
	boost::asio::io_context io;
	async_tcp_server as(io);
	io.run();

	sync_tcp_server s;
	boost::thread t(boost::bind(&sync_tcp_server::run, &s));

	sync_tcp_client clnt;
	clnt.run();

	t.join();
}
