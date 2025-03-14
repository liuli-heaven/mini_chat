#pragma once
#include <iostream>
class Message
{
public:
	Message(short max_len) : total_len_(max_len), cur_len_(0), id_(0) {
		data_ = new char[total_len_ + 1]();
		data_[total_len_] = '\0';
	}
	Message(const char* data, short max_len, short id): total_len_(max_len), id_(id), cur_len_(max_len)
	{
		data_ = new char[total_len_ + 1]();
		memcpy(data_, data, max_len);
		data_[max_len] = '\0';
	}
	~Message() {
		std::cout << "destruct MsgNode" << "\n";
		delete[] data_;
	}

	void Clear() {
		::memset(data_, 0, total_len_);
		cur_len_ = 0;
	}

	short cur_len_;
	short total_len_;
	char* data_;
	short id_;
};

