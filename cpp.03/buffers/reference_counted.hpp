/**
	@file		reference_counted.hpp
	@brief		레퍼런스 카운트 된 버프를 어떻게 만들고 소켓에서 사용하는지
*/
#ifndef __BUFFERS_REFERENCE_COUNTED_HPP__
#define __BUFFERS_REFERENCE_COUNTED_HPP__

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

namespace ba = boost::asio;
namespace bai = boost::asio::ip;

using boost::asio::ip::tcp;

namespace buffers {
	using namespace buffers;

	/**	@class		shared_const_buffer
	*	@brief		수정 불가, 레퍼런스 카운팅 가능한 버퍼
	*/
	class shared_const_buffer {
	private:
		///< 레퍼런스 카운트 버퍼
		boost::shared_ptr<std::vector<char> > data_;

		///< asio const 버퍼
		ba::const_buffer buffer_;

	public:

		explicit shared_const_buffer(const std::string& data)
			: data_(new std::vector<char>(data.begin(), data.end()))
			, buffer_(ba::buffer(*data_))
		{
		}

		// ConstBufferSequence 의 필요에 의한 구현처리
		typedef ba::const_buffer value_type;
		typedef const ba::const_buffer* const_iterator;
		const ba::const_buffer* begin() const { return &buffer_; }
		const ba::const_buffer* end() const { return &buffer_ + 1; }
	};

	/**	@class		session
	*	@brief		통신 세션
	*/
	class session : public boost::enable_shared_from_this<buffers::session> {
	private:
		tcp::socket socket_;
	public:
		session(ba::io_context& io_context) : socket_(io_context) {
		}

		tcp::socket& socket() {
			return socket_;
		}

		void start() {
			shared_const_buffer buffer("Set Time: 1986-03-04 17:32:21");
			ba::async_write(socket_, buffer, boost::bind(
				&buffers::session::handle_write, shared_from_this()));
		}

		void handle_write() {
		}
	};

	typedef boost::shared_ptr<buffers::session> session_ptr;

	/**	@class		server
	*/
	class server
	{
	private:
		ba::io_context& io_context_;
		tcp::acceptor acceptor_;

	public:
		server(ba::io_context& io_context) : io_context_(io_context)
			, acceptor_(io_context, tcp::endpoint(tcp::v4(), 5597)) {

			buffers::session_ptr new_session(new buffers::session(io_context_));
			acceptor_.async_accept(new_session->socket(), boost::bind(
				&buffers::server::handle_accept, this, new_session, ba::placeholders::error));
		}

		void handle_accept(buffers::session_ptr new_session, const boost::system::error_code& e) {
			if (!e) {
				new_session->start();
			}

			new_session.reset(new buffers::session(io_context_));

			acceptor_.async_accept(new_session->socket(), boost::bind(
				&buffers::server::handle_accept, this, new_session, ba::placeholders::error));
		}
	};
}; // namespace buffers

#endif	// __BUFFERS_REFERENCE_COUNTED_HPP__