#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <span>

template <typename T, std::size_t N>
class FixedVector {
public:
    FixedVector() = default;

    enum PushStatus {
        Ok,
        ErrValueTooBig
    };
    PushStatus push(T value) noexcept {
        if (m_current_size + 1 < N) {
            m_backing[m_current_size] = value;
            m_current_size += 1;
            return PushStatus::Ok;
        } else {
            return PushStatus::ErrValueTooBig;
        }
    }

    enum AtStatus {
        Ok,
        ErrInvalidIdx
    };
    AtStatus at(size_t idx, T&& out_value) noexcept {
        if (idx < m_current_size) {
            out_value = m_backing[idx];
            return AtStatus::Ok;
        } else {
            return AtStatus::ErrInvalidIdx;
        }
    }

    inline size_t size() constexpr noexcept {
        return N;
    }

    inline void empty() constexpr noexcept {
        m_current_size = 0;
    }

private:    
    size_t m_current_size = 0;
    T m_backing[N];
};