#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

void print(const boost::system::error_code& e,
	boost::asio::steady_timer* t, int* count)
{
	if (*count < 5)
	{
		std::cout << *count << std::endl;
		++(*count);
		t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
		t->async_wait(boost::bind(print, boost::asio::placeholders::error, t, count));
	}
}

#include <boost/thread/thread.hpp>

// Synchronising completion handlers in multithreaded programs

class printer
{
private:
	boost::asio::strand<boost::asio::io_context::executor_type> strand_;
	boost::asio::steady_timer timer1_;
	boost::asio::steady_timer timer2_;

	int count_;

public:
	printer(boost::asio::io_context& io) : strand_(boost::asio::make_strand(io)),
		timer1_(io, boost::asio::chrono::seconds(1)),
		timer2_(io, boost::asio::chrono::seconds(1)),
		count_(0)
	{
		// 멀티 스레드 환경 하에서 핸들러 함수의 동기화를 위한 strand 의 사용법
		timer1_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print1, this)));
		timer2_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print2, this)));
	}

	~printer()
	{
		std::cout << "Final count is " << count_ << std::endl;
	}

	void print1()
	{
		boost::asio::steady_timer* pt = &timer1_;
		if (count_ < 10)
		{
			std::cout << "Timer 1: " << count_ << std::endl;
			++count_;

			pt->expires_at(pt->expiry() + boost::asio::chrono::seconds(1));
			pt->async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print1, this)));
		}
	}
	void print2()
	{
		boost::asio::steady_timer* pt = &timer2_;
		if (count_ < 10)
		{
			std::cout << "Timer 2: " << count_ << std::endl;
			++count_;

			pt->expires_at(pt->expiry() + boost::asio::chrono::seconds(1));
			pt->async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print2, this)));
		}
	}
};

#define TUTORIAL_ASIO	TutorialAsio