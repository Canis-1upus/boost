#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class sync_tcp_client
{
private:
	boost::asio::io_context io_;
	boost::asio::ip::tcp::socket sock_;

	boost::array<char, 128> buf_;
	boost::system::error_code error_;

public:
	sync_tcp_client() : sock_(io_)
	{
	}

	~sync_tcp_client()
	{
	}

	void run()
	{
		try
		{
			boost::asio::ip::tcp::resolver resolver(io_);

			boost::asio::ip::tcp::resolver::results_type endpoints =
				resolver.resolve("127.0.0.1", "5597");

			boost::asio::connect(sock_, endpoints);

			for (;;)
			{
				size_t len = sock_.read_some(boost::asio::buffer(buf_), error_);

				if (error_ == boost::asio::error::eof)
					break;		// Connection closed cleanly by peer .
				else if (error_)
					throw boost::system::system_error(error_);	// Some other error . 

				std::cout.write(buf_.data(), len);
			}
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
};

#include <ctime>
#include <string>

class sync_tcp_server
{
private:
	boost::asio::io_context io_;
	boost::asio::ip::tcp::acceptor acceptor_;

public:
	sync_tcp_server() : acceptor_(io_,
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 5597))
	{
	}

	void run()
	{
		try
		{
			boost::asio::ip::tcp::socket clnt_sock(io_);
			acceptor_.accept(clnt_sock);

			std::string message = make_daytime_string();

			boost::system::error_code ignored_error;
			boost::asio::write(clnt_sock, boost::asio::buffer(message), ignored_error);
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

private:
	std::string make_daytime_string()
	{
		std::time_t now = time(0);
		return "2022-09-27 15:33:32";
	}
};

// Asynchronous TCP daytime Server
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// enable_shared_from_this, shared_ptr ���, 
// �ش� ��ü�� �����ϴ� operation�� ���� ���� ���� ��ü ������ ����

// enalbe_shared_from_this ���� �Ļ��� ��ü�� ��� �Լ����� shared_from_this �޼��带 ����Ͽ� ����
// shared_ptr �����ڿ� �������� �����ϴ� �ν��Ͻ��� shared_ptr �����ڸ� ����ϴ�.
// �׷��� �ʰ� this�� ����Ͽ� �� shared_ptr�� ���� ��� ���� shared_ptr�� �����ڿ� ������ �ٸ��Ƿ� 
// �߸��� ���� ī��Ʈ�� ������ �� ��.

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
private:
	boost::asio::ip::tcp::socket socket_;
	std::string message_;

public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_context& io_context)
	{
		return pointer(new tcp_connection(io_context));
	}

	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		message_ = "2022-10-06 10:30:22";

		// async_write��  async_write_some �� �޸� ��û�� ��ü ������ ���� ������ �� ������ ���� ����
		boost::asio::async_write(socket_, boost::asio::buffer(message_),
			boost::bind(&tcp_connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

private:
	tcp_connection(boost::asio::io_context& io_context) : socket_(io_context)
	{
	}

	void handle_write(const boost::system::error_code& e, std::size_t sz)
	{
	}
};

class async_tcp_server
{
private:
	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::acceptor acceptor_;

public:
	async_tcp_server(boost::asio::io_context& io) : io_context_(io),
		acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 5597))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		tcp_connection::pointer new_connection = tcp_connection::create(io_context_);

		acceptor_.async_accept(new_connection->socket(), boost::bind(&async_tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
	}

	void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
	{
		if (!error)
		{
			new_connection->start();
		}
		start_accept();
	}
};

// sync_udp_client
class sync_udp_client
{
private:
	boost::asio::io_context io_;
	boost::asio::ip::udp::socket socket_;

	boost::array<char, 1> send_buf_ = { {0} };
	boost::array<char, 128> recv_buf_;

public:
	sync_udp_client() : socket_(io_)
	{}

	~sync_udp_client()
	{}

	void run()
	{
		try
		{
			boost::asio::ip::udp::resolver resolver(io_);
			boost::asio::ip::udp::endpoint receiver_endpoint =
				*resolver.resolve(boost::asio::ip::udp::v4(), "127.0.0.1", "5597").begin();

			socket_.open(boost::asio::ip::udp::v4());
			socket_.send_to(boost::asio::buffer(send_buf_), receiver_endpoint);

			boost::asio::ip::udp::endpoint sender_endpoint;

			std::size_t len = socket_.receive_from(boost::asio::buffer(recv_buf_), sender_endpoint);

			std::cout.write(recv_buf_.data(), len);
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
};

// sync_udp_server
class sync_udp_server
{
private:
	boost::asio::io_context io_;
	boost::asio::ip::udp::socket socket_;

	boost::array<char, 1> recv_buf_;
public:

	sync_udp_server() : socket_(io_, boost::asio::ip::udp::endpoint(
		boost::asio::ip::udp::v4(), 5597))
	{}

	~sync_udp_server()
	{}

	void run()
	{
		try
		{
			for (;;)
			{
				boost::asio::ip::udp::endpoint remote_endpoint;
				socket_.receive_from(boost::asio::buffer(recv_buf_), remote_endpoint);

				std::string message = "2022.09.30 11:22:18";

				boost::system::error_code ignored_error;
				socket_.send_to(boost::asio::buffer(message), remote_endpoint, 0, ignored_error);
			}
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
};

// async_udp_server

class async_udp_server
{
private:
	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint remote_endpoint_;
	boost::array<char, 1> recv_buffer_;

public:
	async_udp_server(boost::asio::io_context& io_context)
		: socket_(io_context, boost::asio::ip::udp::v4(), 5597)
	{
		start_receive();
	}

private:
	void start_receive()
	{
		socket_.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_,
			boost::bind(&async_udp_server::handle_receive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (!error)
		{
			boost::shared_ptr<std::string> message(
				new std::string("2022-09-28 12:02:33"));

			socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
				boost::bind(&async_udp_server::handle_send, this, message,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

			start_receive();
		}
	}

	void handle_send(boost::shared_ptr<std::string> message,
		const boost::system::error_code& error,
		std::size_t bytes_transferred)
	{
	}

};

