#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <span>

/// @brief A Vector with a preallocted amount of values. similar to std::inplace_vector
/// @tparam T The type of data that this vector holds. 
/// @tparam N Number of elenments
template <typename T, std::size_t N>
class FixedVector {
public:
    FixedVector() = default;

    /// @brief Pushes a value onto the vector
    /// @param value the value
    /// @return true if success, false if vector already full
    [[nodiscard]] bool constexpr push(T value) noexcept {
        if (m_current_size + 1 < N) {
            m_backing[m_current_size] = value;
            m_current_size += 1;
            return true;
        } else {
            return false;
        }
    }

    /// @brief gets the value at index
    /// @param idx the index
    /// @param out_value [lifetime object] the output value
    /// @return true if success, false if idx > current size
    [[nodiscard]] bool constexpr at(size_t idx, T& out_value) noexcept {
        if (idx < m_current_size) {
            out_value = m_backing[idx];
            return true;
        } else {
            return false;
        }
    }

    /// @brief gets the current size of this vector
    /// @return the current size of this vector
    inline constexpr size_t size() const noexcept {
        return m_current_size;
    }

    /// @brief Empties this vector
    inline constexpr void empty() noexcept {
        m_current_size = 0;
    }

    /// @brief Gets the underlaying backing data array
    /// @return pointer to the underlaying data array
    inline constexpr T* data() const noexcept {
        return m_backing;
    }

private:    
    size_t m_current_size = 0;
    T m_backing[N];
};