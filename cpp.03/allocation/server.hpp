#include <cstdlib>
#include <iostream>

// 핸들러 기반의 개별 할당 메모리를 관리하기 위한 클래스
// 할당 요청을 위해 반환될 수 있는 메모리 블록을 가짐
// 메모리 할당 요청  시점에 메모리가 사용중이라면, (글로벌) 힙 영역으로 메모리 할당을 위임(할당).

#include <boost/noncopyable.hpp>
#include <boost/aligned_storage.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>

class handler_memory : private boost::noncopyable {
private:
	
	///< Storage space used for handler-based custom memory allocation.
	boost::aligned_storage<1024> storage_;
	///< Whether the handler-based custom allocation storage has been used.
	bool in_use_;
public:
	handler_memory() : in_use_(false) {
	}

	void* allocate(std::size_t size) {
		if (!in_use_ && size < storage_.size) {
			in_use_ = true;
			return storage_.address();
		}
		else {
			return ::operator new(size);
		}
	}

	void deallocate(void* pointer) {
		if (pointer == storage_.address()) {
			in_use_ = false;
		}
		else {
			::operator delete(pointer);
		}
	}
};

// C++03 에서 사용하기 위한 C++11의 allocator 의 최소 요구 사항만을 만족 시킨 구현
template<typename T>
class handler_allocator {
public:	// private:
	///< The underlying memory.
	handler_memory& memory_;

public:
	typedef T value_type;

	explicit handler_allocator(handler_memory& mem) : memory_(mem) {
	}

	template<typename U>
	handler_allocator(const handler_allocator<U>& other) : memory_(other.memory_) {
	}

	template <typename U>
	struct rebind {
		typedef handler_allocator<U> other;
	};

	bool operator ==(const handler_allocator& other) const {
		return &memory_ == &other.memory_;
	}

	bool operator !=(const handler_allocator& other) const {
		return &memory_ != &other.memory_;
	}

	T* allocate(std::size_t n) const {
		return static_cast<T*>(memory_.allocate(sizeof(T) * n));
	}

	void deallocate(T* p, std::size_t /*n*/) const {
		return memory_.deallocate(p);
	}

};

class session : public boost::enable_shared_from_this<session> {
private:
	///< The socket used to communicate with the client.
	boost::asio::ip::tcp::socket socket_;

	///< Buffer used to store data received from the client.
	boost::array<char, 1024> data_;

	///< The memory to use for handler-based custom memory allocation.
	handler_memory handler_memory_;

public:
	session(boost::asio::io_context& io_context) : socket_(io_context) {
	}

	boost::asio::ip::tcp::socket& socket() {
		return socket_;
	}

	void start() {
		socket_.async_read_some(boost::asio::buffer(data_),
			boost::asio::bind_allocator(
				handler_allocator<int>(handler_memory_),
				boost::bind(&session::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)));
	}

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
		if (!error) {
			boost::asio::async_write(socket_,
				boost::asio::buffer(data_, bytes_transferred),
				boost::asio::bind_allocator(
					handler_allocator<int>(handler_memory_),
					boost::bind(&session::handle_write,
						shared_from_this(),
						boost::asio::placeholders::error)));
		}
	}

	void handle_write(const boost::system::error_code& error) {
		if (!error) {
			socket_.async_read_some(boost::asio::buffer(data_),
				boost::asio::bind_allocator(
					handler_allocator<int>(handler_memory_),
					boost::bind(&session::handle_read, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)));
		}
	}
};

typedef boost::shared_ptr<session> session_ptr;

class server {
private:
	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::acceptor acceptor_;

public:

	server(boost::asio::io_context& io_context, short port) : io_context_(io_context),
		acceptor_(
			io_context, 
			boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
		session_ptr new_session(new session(io_context_));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error));
	}

	void handle_accept(session_ptr new_session, const boost::system::error_code& error) {
		if (!error) {
			new_session->start();
		}

		new_session.reset(new session(io_context_));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}
};
