#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! all `uint64_t` indices does not include SYN & FIN
//! remember to take these two into consideration
void TCPReceiver::segment_received(const TCPSegment &seg) {
    WrappingInt32 seqno{seg.header().seqno};
    constexpr uint64_t RESERVE_FOR_SYN = 1;

    if (seg.header().syn) {
        _state = SYN_RECV;
        _isn = seqno;
    }
    if (seg.header().fin && _state == SYN_RECV) {
        _state = FIN_RECV;
    }

    if (_isn == seqno && !seg.header().syn) {
        return ;
    }
    
    if (_state != LISTEN) {
        //! after the 1st segment, all `abs_seqno` should decrease 1 to eliminate SYN byte
        uint64_t abs_seqno = unwrap(seqno, _isn, _checkpoint);
        abs_seqno -= (abs_seqno != 0);
        
        _checkpoint = abs_seqno;
        _reassembler.push_substring(seg.payload().copy(), abs_seqno, _state == FIN_RECV);
        _ackno = wrap(_reassembler.ilast(), _isn) + RESERVE_FOR_SYN + _reassembler.reassemble_ended();
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const { 
    if (_state == LISTEN) {
        return {};
    } else {
        return _ackno;
    }
}

size_t TCPReceiver::window_size() const { 
    return _reassembler.stream_out().remaining_capacity(); 
}
