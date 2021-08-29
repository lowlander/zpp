//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_ATOMIC_BITSET_HPP
#define ZPP_INCLUDE_ZPP_ATOMIC_BITSET_HPP

#include <sys/atomic.h>
#include <sys/__assert.h>

#include <cstddef>

namespace zpp {

///
/// @brief class wrapping an atomic_var_t array
///
/// @param BitsetSize the size of the bitset in bits
///
template<size_t T_BitsetSize>
class atomic_bitset {
public:
  ///
  /// @brief default constructor
  ///
  /// The default constructor initializes the atomic_bitset
  /// to all bits set to 0.
  ///
  constexpr atomic_bitset() noexcept = default;

  ///
  /// @brief return size of the bitset
  ///
  /// @return the size of the bitset in bits
  ///
  constexpr size_t bit_count() const noexcept
  {
    return T_BitsetSize;
  }

  ///
  /// @brief atomically get a bit from the bitset
  ///
  /// @param bit the index of the bit to return
  ///
  /// @return the requested bit value
  ///
  [[nodiscard]] bool load(size_t bit) const noexcept
  {
    __ASSERT_NO_MSG(bit < T_BitsetSize);
    return atomic_test_bit(m_var, bit);
  }

  ///
  /// @brief atomically set a bit a value
  ///
  /// @param bit the index of the bit to set
  /// @param val the value the bit should be set to
  ///
  void store(size_t bit, bool val) noexcept
  {
    __ASSERT_NO_MSG(bit < T_BitsetSize);
    atomic_set_bit_to(m_var, bit, val);
  }

  ///
  /// @brief atomically set a bit to true/1
  ///
  /// @param bit the index of the bit to set
  ///
  void set(size_t bit) noexcept
  {
    __ASSERT_NO_MSG(bit < T_BitsetSize);
    atomic_set_bit(m_var, bit);
  }

  ///
  /// @brief atomically set a bit to false/0
  ///
  /// @param bit the index of the bit to set
  ///
  void clear(size_t bit) noexcept
  {
    __ASSERT_NO_MSG(bit < T_BitsetSize);
    atomic_clear_bit(m_var, bit);
  }

  ///
  /// @brief atomically clear a bit while returning the previous value.
  ///
  /// @param bit the index of the bit to set
  ///
  /// @return the bit value before it was cleared
  ///
  [[nodiscard]] bool fetch_and_clear(size_t bit) noexcept
  {
    __ASSERT_NO_MSG(bit < T_BitsetSize);
    return atomic_test_and_clear_bit(m_var, bit);
  }

  ///
  /// @brief atomically set a bit while returning the previous value.
  ///
  /// @param bit the index of the bit to set
  ///
  /// @return the bit value before it was set
  ///
  [[nodiscard]] bool fetch_and_set(size_t bit) noexcept
  {
    __ASSERT_NO_MSG(bit < T_BitsetSize);
    return atomic_test_and_set_bit(m_var, bit);
  }
private:
  ATOMIC_DEFINE(m_var, T_BitsetSize) {};
public:
  atomic_bitset(const atomic_bitset&) = delete;
  atomic_bitset(atomic_bitset&&) = delete;
  atomic_bitset& operator=(const atomic_bitset&) = delete;
  atomic_bitset& operator=(atomic_bitset&&) = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_ATOMIC_BITSET_HPP
