#include "stream_reassembler.hh"

#include <limits>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _iend(-1), _set(), _ilast(), _nbytes_holding() {}

void StreamReassembler::append(const string &data, const uint64_t index) {
    uint64_t _pos = _ilast - index;
    if (_pos >= data.length()) {
        return ;
    }
    _ilast += _output.write(data.substr(_ilast - index));
}

bool StreamReassembler::overlap(const segment_t &s1, const segment_t &s2) {
    auto i1 = s1.first, i2 = s2.first;
    if ((i1.first <= i2.first && i2.first <= i1.second)
     || (i2.first <= i1.first && i1.first <= i2.second)) {
        return true;
     }
    return false;
}

StreamReassembler::segment_t StreamReassembler::merge(const segment_t &s1, const segment_t &s2) {
    auto i1 = s1.first, i2 = s2.first;
    if (i1.first > i2.first) {
        return merge(s2, s1);
    }
    // It is guaranteed that i1.first <= i2.first
    if (i2.second <= i1.second) {
        // s1 contains s2
        _nbytes_holding -= s2.second.length();
        return s1;
    } else {
        // s1; s2
        _nbytes_holding -= i1.second - i2.first;
        return std::make_pair(
            std::make_pair(i1.first, i2.second),
            s1.second + s2.second.substr(i1.second - i2.first)
        );
    }
}

void StreamReassembler::merge_left(decltype(_set)::iterator &it) {
    segment_t cur{*it}, pre{};

    while (it != _set.begin()) {
        pre = *(-- it);
        if (overlap(pre, cur)) {
            _set.erase(pre);
            _set.erase(cur);
            it = _set.insert(merge(pre, cur)).first;
        } else {
            break;
        }
    }
}

void StreamReassembler::merge_right(decltype(_set)::iterator &it) {
    segment_t cur{*it}, nex{};

    while ((++ it) != _set.end()) {
        nex = *it;
        if (overlap(nex, cur)) {
            _set.erase(nex);
            _set.erase(cur);
            it = _set.insert(merge(nex, cur)).first;
        } else {
            break;
        }
    }
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const uint64_t index, const bool eof) {
    if (eof) {
        _iend = data.length() + index;
    }

    if (index <= _ilast) {
        append(data, index);

        while (!_set.empty() && _set.begin()->first.first <= _ilast) {
            auto top = _set.begin();
            _nbytes_holding -= top->second.length();
            append(top->second, top->first.first);
            _set.erase(top);
        }
    } else {
        size_t _len = std::min(data.length(), _output.remaining_capacity());
        if (_len > 0) {
            auto it{_set.insert(make_pair(make_pair(index, index + _len), data.substr(0, _len))).first};
            merge_left(it);
            merge_right(it);
            _nbytes_holding += _len;
        }
    }
    
    if (_ilast >= _iend) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _nbytes_holding; }

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
