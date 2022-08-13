#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return WrappingInt32{static_cast<uint32_t>(n) + isn.raw_value()};
}

static uint64_t dist(uint64_t x, uint64_t checkpoint) {
    return (x > checkpoint) ? (x - checkpoint) : (checkpoint - x);
}

static void update(uint64_t &ans, uint64_t checkpoint, uint64_t cand) {
    if (dist(ans, checkpoint) > dist(cand, checkpoint)) {
        ans = cand;
    }
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    constexpr uint64_t MASK  = 0xFFFFFFFF00000000ULL;
    constexpr uint64_t DELTA = 0x0000000100000000ULL;

    uint64_t delta = n.raw_value() - isn.raw_value();
    // abs_seq - delta = k * 2^32, where k∈ℕ

    auto pos = checkpoint & MASK;
    uint64_t ans{pos + delta};

    if (pos > 0) {
        update(ans, checkpoint, pos - DELTA + delta);
    }
    if (pos != MASK) {
        update(ans, checkpoint, pos + DELTA + delta);
    }
    return ans;
}
