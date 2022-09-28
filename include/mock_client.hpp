#include <boost/asio.hpp>
#include <boost/array.hpp>

#define MAX_BUFF_LEN	1024

class tcp_client {
private:
//protected:
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

	virtual void connect(boost::asio::ip::tcp::endpoint end_point) = 0;
	virtual void send_message() = 0;
	virtual void read_message() = 0;
	virtual void close_connection() = 0;	// io_context_.post()?

	virtual void handle_connected() = 0;
	virtual void handle_sent() = 0;
	virtual void handle_received() = 0;

};

class fake_tcp_client : public tcp_client {
private:
public:
	fake_tcp_client(boost::asio::io_context& io_context) : tcp_client(io_context) {
	}

	virtual void connect(boost::asio::ip::tcp::endpoint end_point) {
	}

	virtual void send_message() {
		socket_.is_open();
	}
	virtual void read_message() {}
	virtual void close_connection() {}

	virtual void handle_connected() {}
	virtual void handle_sent() {}
	virtual void handle_received() {}
};

class mock_tcp_client : public tcp_client {
private:
	fake_tcp_client client_;
public:
	// MOCK_METHOD

	void delegate_to_fake() {
		// ON_CALL(*this, func()).WillByDefault(Invoke(&client_, &fake_tcp_client::func));
	}
};

