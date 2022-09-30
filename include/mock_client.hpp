#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <gmock/gmock.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-matchers.h>

#define MAX_BUFF_LEN	1024

class tcp_client {
private:
protected:
public:
	boost::array<unsigned char, MAX_BUFF_LEN> send_buf_;
	boost::array<unsigned char, MAX_BUFF_LEN> recv_buf_;

	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::socket socket_;
public:
	tcp_client(boost::asio::io_context& io_context)
		: io_context_(io_context)
		, socket_(io_context)
		, send_buf_({})
		, recv_buf_({})
	{
	}

	virtual void connect(boost::asio::ip::tcp::endpoint endpoint) = 0;
	virtual void send_message() = 0;
	virtual void read_message() = 0;
	virtual void close_connection() = 0;	// io_context_.post()?

	virtual void handle_connected(const boost::system::error_code& e) = 0;
	virtual void handle_sent() = 0;
	virtual void handle_received() = 0;

};

class fake_tcp_client : public tcp_client {
private:
public:
	fake_tcp_client(boost::asio::io_context& io_context) : tcp_client(io_context) {
	}

	virtual void connect(boost::asio::ip::tcp::endpoint endpoint) {
		socket_.async_connect(endpoint, 
			boost::bind(&fake_tcp_client::handle_connected, this, 
				boost::asio::placeholders::error));
	}

	virtual void send_message() {
	}
	virtual void read_message() {}
	virtual void close_connection() {}

	virtual void handle_connected(const boost::system::error_code& e) {
		if (e) {
			std::cout << e.what() << std::endl;;
		}
	}
	virtual void handle_sent() {}
	virtual void handle_received() {}

	void set_send_data(std::string data) {
		std::copy(data.begin(), data.end(), send_buf_.begin());
	}
};

class mock_tcp_client : public tcp_client {
private:
	fake_tcp_client client_;
public:
	mock_tcp_client(boost::asio::io_context& io_context) : client_(io_context), tcp_client(io_context) {
		delegate_to_fake();
	}

	MOCK_METHOD1(connect, void(boost::asio::ip::tcp::endpoint endpoint));
	MOCK_METHOD1(handle_connected, void(const boost::system::error_code& e));

	virtual void send_message() {
	}
	virtual void read_message() {}
	virtual void close_connection() {}

	virtual void handle_sent() {}
	virtual void handle_received() {}

	void delegate_to_fake() {
		ON_CALL(*this, connect(::testing::_))
			.WillByDefault(::testing::Invoke(&client_, &fake_tcp_client::connect));
		ON_CALL(*this, handle_connected(::testing::_))
			.WillByDefault(::testing::Invoke(&client_, &fake_tcp_client::handle_connected));
	}

	void set_send_data(std::string data) {
		client_.set_send_data(data);
	}

	boost::array<unsigned char, 1024>& get_send_data() {
		return client_.send_buf_;
	}
};

