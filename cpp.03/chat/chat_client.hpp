/**	@file		chat_client.hpp
*/
#include <deque>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

#include "chat_message.hpp"

namespace chat {
	namespace ba = boost::asio;
	namespace bai = boost::asio::ip;

	using boost::asio::ip::tcp;

	using namespace chat;

	typedef std::deque<chat_message> chat_message_queue;

	class chat_client {
	private:
		ba::io_context& io_context_;
		tcp::socket socket_;
		chat_message read_message_;
		chat_message_queue write_messages_;

	public:
		chat_client(ba::io_context& io_context, const tcp::resolver::results_type& endpoints)
			: io_context_(io_context), socket_(io_context) {
			ba::async_connect(socket_, endpoints, boost::bind(
				&chat_client::handle_connect, this, ba::placeholders::error));
		}

		void write(const chat_message& msg) {
			ba::post(io_context_, boost::bind(&chat_client::do_write, this, msg));
		}

		void close() {
			ba::post(io_context_, boost::bind(&chat_client::do_close, this));
		}

	private:
		void handle_connect(const boost::system::error_code& error) {
			if (!error) {
				ba::async_read(socket_, ba::buffer(read_message_.data(), chat_message::header_length),
					boost::bind(&chat_client::handle_read_header, this, ba::placeholders::error));
			}
		}

		void handle_read_header(const boost::system::error_code& error) {
			if (!error && read_message_.decode_header()) {
				ba::async_read(socket_, ba::buffer(read_message_.body(), read_message_.body_length()),
					boost::bind(&chat_client::handle_read_body, this, ba::placeholders::error));
			}
			else {
				do_close();
			}
		}

		void handle_read_body(const boost::system::error_code& error) {
			if (!error) {
				std::cout.write(read_message_.body(), read_message_.body_length());
				std::cout << "\n";

				ba::async_read(socket_, ba::buffer(read_message_.data(), chat_message::header_length),
					boost::bind(&chat_client::handle_read_header, this, ba::placeholders::error));
			}
			else {
				do_close();
			}
		}

		void do_close() {
			socket_.close();
		}

		void do_write(chat_message msg) {
			bool write_in_progress = !write_messages_.empty();
			write_messages_.push_back(msg);

			if (!write_in_progress) {
				ba::async_write(socket_, ba::buffer(
					write_messages_.front().data(), write_messages_.front().length()),
					boost::bind(&chat_client::handle_write, this, ba::placeholders::error));
			}
		}

		void handle_write(const boost::system::error_code& error) {
			if (!error) {
				write_messages_.pop_front();
				if (!write_messages_.empty()) {
					ba::async_write(socket_, ba::buffer(write_messages_.front().data(),
						write_messages_.front().length()),
						boost::bind(&chat_client::handle_write, this, ba::placeholders::error));
				}
			}
			else {
				do_close();
			}
		}

	};
};

/*

int main(int argc, char* argv[]) {
	try {
		if (argc != 3) {
			std::cerr << "Usage: chat_client <host> <port>\n";
			return 1;
		}

		boost::asio::io_context io_context;

		boost::asio::ip::tcp::resolver resolver(io_context);
		boost::asio::ip::tcp::resolver::results_type endpoints
			= resolver.resolve("127.0.0.1", "5597");

		chat::chat_client c(io_context, endpoints);

		boost::thread t(boost::bind(&boost::asio::io_context::run, &io_context));

		char line[chat::chat_message::max_body_length + 1];

		while (std::cin.getline(line, chat::chat_message::max_body_length + 1)) {
			using namespace std; 
			chat::chat_message msg;
			msg.body_length(strlen(line));
			memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			c.write(msg);
		}

		c.close();
		t.join();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return 0;
}

*/

