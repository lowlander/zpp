///
/// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_UTILS_HPP
#define ZPP_INCLUDE_ZPP_UTILS_HPP

#include <cstdint>

namespace zpp {

///
/// @brief check if the instances refere to the same native zephyr
///        object handle.
///
/// @return true if they refer to the same native zephyr object.
///
template<class T_LHS, class T_RHS>
constexpr bool operator==(const T_LHS& lhs, const T_RHS& rhs) noexcept
{
  if constexpr (std::is_pointer_v<T_LHS> && std::is_pointer_v<T_RHS>) {
    return lhs == rhs;
  }

  if constexpr (std::is_pointer_v<T_LHS> && !std::is_pointer_v<T_RHS>) {
    return lhs == rhs.native_handle();
  }

  if constexpr (!std::is_pointer_v<T_LHS> && std::is_pointer_v<T_RHS>) {
    return lhs.native_handle() == rhs;
  }

  if constexpr (!std::is_pointer_v<T_LHS> && !std::is_pointer_v<T_RHS>) {
    return lhs.native_handle() == rhs.native_handle();
  }

  static_assert(true, "should never be reached");
  return true;
}

///
/// @brief check if the arguments refere to different native zephyr
///        object handles.
///
/// @return true if they refer to different native zephyr objects.
///
template<class T_LHS, class T_RHS>
constexpr bool operator!=(const T_LHS& lhs, const T_RHS& rhs) noexcept
{
  if constexpr (std::is_pointer_v<T_LHS> && std::is_pointer_v<T_RHS>) {
    return lhs != rhs;
  }

  if constexpr (std::is_pointer_v<T_LHS> && !std::is_pointer_v<T_RHS>) {
    return lhs != rhs.native_handle();
  }

  if constexpr (!std::is_pointer_v<T_LHS> && std::is_pointer_v<T_RHS>) {
    return lhs.native_handle() != rhs;
  }

  if constexpr (!std::is_pointer_v<T_LHS> && !std::is_pointer_v<T_RHS>) {
    return lhs.native_handle() != rhs.native_handle();
  }

  static_assert(true, "should never be reached");
  return false;
}

///
/// @brief calculate a power of 2
///
/// @param power the power of 2 to calculate
///
/// @return the power of 2
///
consteval uint32_t power_of_two(uint32_t power) noexcept
{
  uint32_t res = 1;

  for (uint32_t i = 1; i <= power; ++i) {
    res *= 2;
  }

  return res;
}

///
/// @brief check if a value is a power of two
///
/// @param value the value to check
///
/// @return true if @a value is a power of two
///
consteval bool is_power_of_two(uint32_t value) noexcept
{
  uint32_t power {0};
  uint32_t calc_val;

  do {
    calc_val = power_of_two(power++);
  } while (calc_val < value);

  return calc_val == value;
}

///
/// @brief Check if a value is a multiple of another value
///
/// @param value the value to check
/// @param base the base the @a value should be a multiple of
///
/// @return true if @a value is a multiple of @a base
///
consteval bool is_multiple_of(uint32_t value, uint32_t base)
{
  if (value == 0 || base == 0) {
    return false;
  } else {
    return (value % base) == 0;
  }
}

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_UTILS_HPP
