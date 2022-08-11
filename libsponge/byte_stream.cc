#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) { _capacity = capacity; }

size_t ByteStream::write(const string &data) {
    size_t _len{std:: min(data.length(), remaining_capacity())};
    _bytes_written += _len;

    for (size_t i{}; i < _len; ++ i) {
        _buffer.push_back(data[i]);
    }
    return _len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t _len{std:: min(len, _buffer.size())};
    std:: string result{};

    for (size_t i = 0; i < _len; ++ i) {
        result += _buffer[i];
    }
    return result;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    size_t _len{std:: min(len, _buffer.size())};
    _bytes_read += _len;

    for (; _len --; ) {
        _buffer.pop_front();
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std:: string result{peek_output(len)};
    pop_output(len);
    _bytes_read += result.length();
    return result;
}

void ByteStream::end_input() { _ended = true; }

bool ByteStream::input_ended() const { return _ended; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return buffer_size() == 0; }

bool ByteStream::eof() const { return input_ended() && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }
