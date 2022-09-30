/**	@file		chat_message.hpp
*/

#ifndef __CHAT_CHAT_MESSAGE_HPP__
#define	__CHAT_CHAT_MESSAGE_HPP__

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace chat {
	class chat_message {
	public:
		enum { header_length = 4, max_body_length = 512 };
	private:
		char data_[header_length + max_body_length];
		std::size_t body_length_;
	public:

		chat_message() : body_length_(0) {
		}

		const char* data() const { return data_; }

		char* data() { return data_; }

		std::size_t length() const { return (header_length + body_length_); }

		const char* body() {
			return data_ + header_length;
		}

		std::size_t body_length() const { return body_length_; }

		void body_length(std::size_t new_length) {
			body_length_ = new_length;

			if (body_length_ > max_body_length)
				body_length_ = max_body_length;
		}

		bool decode_header() {
			using namespace std;
			char header[header_length + 1] = "";
			strncat(header, data_, header_length);

			body_length_ = atoi(header);
			if (body_length_ > max_body_length)
			{
				body_length_ = 0;
				return false;
			}
			return true;
		}

		void encode_header() {
			using namespace std;
			char header[header_length + 1] = "";
			sprintf(header, "%4d", static_cast<int>(body_length_));
			memcpy(data_, header, header_length);
		}
	};
};

#endif	// __CHAT_CHAT_MESSAGE_HPP__