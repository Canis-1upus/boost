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

	FEATURE("asio의 io동작에 대한 proactor 동작을 확인한다");
	SCENARIO("asio 서비스에 정의된 동기화 타이머를 작동 해당 함수의 반환까지 걸린 시각이 세팅된 시간과 동일한지 비교한다");

	GIVEN("타이머 세팅 시각은 5(s) 이다");
	uint32_t sec = 5;
	system_timer timer(io, seconds(sec));

	WHEN("타이머 시작 시간을 체크하고, 타이머를 시작. 완료 시각을 체크");
	system_clock::time_point start = system_clock::now();
	timer.wait();
	system_clock::time_point end = system_clock::now();

	THEN("타이머 만료까지 걸린 시각은 세팅 시각과 같거나 크게 체크될 것이다");

	// 반올림 정확도 부정확
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

	FEATURE("asio의 io동작에 대한 proactor 동작을 확인한다");

	SCENARIO("asio 서비스에 정의된 비동기화 타이머를 작동"); 
	SCENARIO("해당 함수의 반환까지 걸린 시각이 세팅된 시간보다 작을 것이며,");
	SCENARIO("asio는 io_context::run()을 수행한 스레드로 io 완료 이후 등록된 함수를 수행할 것이기에");
	SCENARIO("Mock Test로 동작에 대한 테스트 확인이 가능하다");

	GIVEN("타이머 세팅 시각은 5(s) 이다");
	uint32_t sec = 5;
	system_timer timer(io, seconds(sec));

	WHEN("타이머 시작 시간을 체크하고, 타이머를 시작. 완료 시각을 체크");
	system_clock::time_point start = system_clock::now();

	//AND("Mock 테스트로 io 완료 이후 해당 함수가 호출될지를 기대 확인");
	//MockAsyncTimer mock;
	//EXPECT_CALL(mock, endPrint).Times(1);
	//std::function<void(const boost::system::error_code&)> memberFunc
	//  = std::bind(&MockAsyncTimer::endPrint, std::placeholders::_1);
	//timer.async_wait(&memberFunc(mock));

	timer.async_wait(&EndPrint);

	system_clock::time_point end = system_clock::now();

	THEN("타이머 만료까지 걸린 시각은 세팅 시각이 수행에 체크된 시간보다 클 것이다.(비동기로 바로 반환이 발생하기에)");
	// EXPECT_LE(exp, act) := (exp <= act)

	RESULT(boost::lexical_cast<std::string>(std::chrono::round<seconds>(end - start).count()));
	EXPECT_GE(sec, duration_cast<seconds>(end - start).count());
}

TEST(TUTORIAL_ASIO, BindArgumentToCompletionHandler)
{
	FEATURE("asio, io 핸들러 함수에 추가적인 파라미터를 바인드 할 수 있음을 보여준다");

	SCENARIO("print 함수 내 만료 타이머를 갱신하며 비동기 IO를 계속 수행토록 한다");
	SCENARIO("5초 이상의 시점에서는 더 이상 비동기 IO 작업이 발생하지 않으므로 더 이상 작업이 없는 상태로");
	SCENARIO("io::run의 반환이 발생");

	boost::asio::io_context io;
	int count = 0;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
	t.async_wait(boost::bind(print, boost::asio::placeholders::error, &t, &count));
	io.run();

	std::cout << "Final count is " << count << std::endl;
}

TEST(TUTORIAL_ASIO, SynchronisingCompletionHandlersInMTP)
{
	FEATURE("멀티스레드 환경에서 Completion 핸들러들의 동기화를 위한 strand의 사용법");

	SCENARIO("기본적으로 Completion 핸들러는 오직 io_context::run 이 호출된 thread에 의해서만 호출 되도록 보장");
	SCENARIO("따라서 io_context::run 이 단일 스레드에서만 호출 된다는 것은 Completeion 핸들러에 대한 동시성을 확보할 수 없다");
	SCENARIO("결국 io_context::run을 스레드 풀에서 돌려야 한다는 이야기인데, 이럴 경우, Completion 핸들러가 멀티 스레드에 의해 동시에 실행 되므로, Completion 핸들러 내의 thread-unsafe 한 공유 자원들에 대한 동기화 방법이 필요하다");
	SCENARIO("이를 위해 간단히 제공되는 방식이 strand 클래스 템플릿 인것 ");
	SCENARIO("strand 는 executor 어댑터로, 이것을 통해 dispatch(파견)ed 되는 핸들러들에 대해 동기화를 보장 한다");
	SCENARIO("실행 중인 핸들러가 다른 핸들러가 실행되기 전에 반드시 먼저 완료 될 수 있도록 보장하는 식(이때 2개의 핸들러 모두 strand 어댑터를 통해 등록 된 것이어야 한다)");
	SCENARIO("동일 strand 롤 통해 등록된 핸들러에 대한 동시성은 비록 깨지게 되는 것이지만, 여전히 다른 strand 혹은 직접 등록된 다른 Completion 핸들러들에 대한 동시성은 보장한다");

	SCENARIO("비 동기 동작을 초기화 할 때, 각 핸들러들은 bind_executor 함수로 strand 객체와 바인드 되어 전달 될 수 있도록 한다");

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
